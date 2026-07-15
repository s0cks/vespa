const std = @import("std");
const c = @cImport({
  @cInclude("uv.h");
  @cInclude("worker.h");
});

pub const WorkerEventLoop = struct {
  allocator: std.mem.Allocator,
  loop: *c.uv_loop_t,
  worker: *c.Worker,

  pub fn init(allocator: std.mem.Allocator, filename: [*:0]const u8) !WorkerEventLoop {
    std.debug.print("creating event loop...\n", .{});
    const loop = try allocator.create(c.uv_loop_t);
    if (c.uv_loop_init(loop) < 0) {
      return error.LoopInitFailed;
    }

    const worker: *c.Worker = c.NewWorker(loop, filename) orelse {
      return error.CAllocationFailed;
    };

    return WorkerEventLoop {
      .allocator = allocator,
      .loop = loop,
      .worker = worker,
    };
  }

  pub fn deinit(self: WorkerEventLoop) void {
    _ = c.WorkerFree(self.worker);
    _ = c.uv_loop_close(self.loop);
    self.allocator.destroy(self.loop);
  }

  pub fn call_init(self: WorkerEventLoop) !bool {
    if(!c.SandboxCallInit(self.worker.sandbox)) {
      return error.FailedToCallInit;
    }

    return true;
  }

  pub fn call_view(self: WorkerEventLoop) !bool {
    if(!c.SandboxCallView(self.worker.sandbox)) {
      return error.FailedToCallInit;
    }

    return true;
  }

  pub fn call_update(self: WorkerEventLoop) !bool {
    if(!c.SandboxCallUpdate(self.worker.sandbox)) {
      return error.FailedToCallInit;
    }

    return true;
  }

  pub fn run(self: WorkerEventLoop, mode: c_uint) bool {
    std.debug.print("running event loop...\n", .{});
    _ = c.uv_run(self.loop, mode);
    std.debug.print("event loop finished.\n", .{});
    return true;
  }
};
