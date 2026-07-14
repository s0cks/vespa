const std = @import("std");
const Io = std.Io;
const c = @cImport({
  @cInclude("uv.h");
  @cInclude("supervisor.h");
});

const AppContext = struct {
  count: u32 = 0,
};

fn timerCallback(handle: [*c]c.uv_timer_t) callconv(.c) void {
  const ctx: *AppContext = @ptrCast(@alignCast(handle.*.data));
  ctx.count += 1;

  std.debug.print("Timer ticked! Count: {d}\n", .{ctx.count});

  if (ctx.count >= 3) {
    std.debug.print("Stopping timer and closing handle.\n", .{});
    _ = c.uv_timer_stop(handle);
    c.uv_close(@ptrCast(handle), null); 
  }
}

pub fn main(init: std.process.Init) !u8 {
  const environ = init.environ_map;
  if (environ.get("PATH")) |path| {
    std.debug.print("PATH: {s}\n", .{path});
  } else {
    std.debug.print("PATH variable is not set.\n", .{});
  }

  const worker_path_slice = environ.get("VESPA_QUEEN_HIVE_PATH");
  var c_worker_path: [*c]u8 = null;
  if (worker_path_slice) |slice| {
    const mutable_copy = try init.gpa.dupeZ(u8, slice);
    c_worker_path = @ptrCast(mutable_copy.ptr);
  }
  defer {
    if (c_worker_path != null) {
      init.gpa.free(std.mem.span(c_worker_path));
    }
  }

  var config: c.SupervisorConfig = c.SupervisorConfig {
    .hive_bin = c_worker_path,
  };
  const supervisor_ptr: ?*c.Supervisor = c.NewSupervisor(&config);
  const supervisor = supervisor_ptr orelse {
    return error.CAllocationFailed;
  };
  defer c.SupervisorFree(supervisor);

  const result = c.SupervisorRunDefault(supervisor);
  if (!result) {
    std.debug.print("error: failed to run supervisor.\n", .{});
    return 1; // EXIT_FAILURE
  }

  return 0; // EXIT_SUCCESS
}
