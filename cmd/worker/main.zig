const std = @import("std");

// Assuming these are your custom C headers translated or exposed to Zig
const c = @cImport({
  @cInclude("worker.h");
});

pub fn main(init: std.process.Init) u8 {
  const args = init.minimal.args.toSlice(init.arena.allocator()) catch {
    std.log.err("failed to allocate memory for arguments",.{});
    return 1;
  };

  if (args.len < 2) {
    std.log.err("expected filename argument", .{});
    return 1; // EXIT_FAILURE
  }

  const filename: [*:0]const u8 = args[1];
  var worker: c.Worker = undefined;
  if (!c.WorkerInit(&worker)) {
    std.log.err("failed to initialize worker", .{});
    return 1;
  }
  defer c.WorkerFree(&worker);

  if (!c.WorkerLoadWasm(&worker, filename)) {
    return 1;
  }

  if (!c.WorkerWidgetInit(&worker)) {
    std.log.err("failed to call widget 'init' function", .{});
    return 1;
  }

  if (!c.WorkerRunDefault(&worker)) {
    std.log.err("failed to run worker", .{});
    return 1;
  }

  return 0; // EXIT_SUCCESS
}
