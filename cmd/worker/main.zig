const std = @import("std");

const c = @cImport({
  @cInclude("uv.h");
  @cInclude("worker.h");
});

fn on_idle(handle: [*c]c.uv_idle_t) callconv(.c) void {
  _ = handle;
  // if (handle.*.data) |data_ptr| {
  //   const context = @as(*i32, @ptrCast(@alignCast(data_ptr)));
  //   std.debug.print("Context data value: {}\n", .{context.*});
  // }
  //
  // uv.uv_close(@ptrCast(handle), null);
  //
  // if (worker->has_updates) {
  //   bool success = WorkerWidgetUpdate(worker);
  // }
}

fn on_check(handle: [*c]c.uv_check_t) callconv(.c) void {
  _ = handle;
  // if (worker->has_updates) {
  //   bool success = WorkerWidgetView(worker);
  //   worker->has_updates = false;
  // }
}

pub fn main(init: std.process.Init) !u8 {
  const allocator = std.heap.page_allocator;
  const args = init.minimal.args.toSlice(init.arena.allocator()) catch {
    std.log.err("failed to allocate memory for arguments",.{});
    return 1;
  };

  if (args.len < 2) {
    std.log.err("expected filename argument", .{});
    return 1; // EXIT_FAILURE
  }

  const filename: [*:0]const u8 = args[1];

  std.debug.print("creating event loop...\n", .{});
  const loop = try allocator.create(c.uv_loop_t);
  defer allocator.destroy(loop);
  if (c.uv_loop_init(loop) < 0) {
    return error.LoopInitFailed;
  }
  defer _ = c.uv_loop_close(loop);

  var worker: c.Worker = undefined;
  if (!c.WorkerInit(&worker, loop)) {
    std.log.err("failed to initialize worker", .{});
    return 1;
  }
  defer c.WorkerFree(&worker);

  var idle: c.uv_idle_t = undefined;
  if(c.uv_idle_init(loop, &idle) < 0) {
    return error.IdleInitFailed;
  }
  idle.data = &worker;

  if(c.uv_idle_start(&idle, on_idle) < 0) {
    return error.IdleStartFailed;
  }

  var check: c.uv_check_t = undefined;
  if(c.uv_check_init(loop, &check) < 0) {
    return error.CheckInitFailed;
  }
  check.data = &worker;

  if(c.uv_check_start(&check, on_check) < 0) {
    return error.CheckStartFailed;
  }

  if (!c.WorkerLoadWasm(&worker, filename)) {
    return 1;
  }

  if (!c.WorkerWidgetInit(&worker)) {
    std.log.err("failed to call widget 'init' function", .{});
    return 1;
  }

  std.debug.print("running event loop...\n", .{});
  _ = c.uv_run(loop, c.UV_RUN_DEFAULT);
  std.debug.print("event loop finished.\n", .{});
  return 0; // EXIT_SUCCESS
}
