const std = @import("std");
const el = @import("src/event_loop.zig");
const c = @cImport({
  @cInclude("uv.h");
  @cInclude("worker.h");
});

pub fn main(init: std.process.Init) !u8 {
  const allocator = std.heap.c_allocator;
  const args = init.minimal.args.toSlice(init.arena.allocator()) catch {
    std.log.err("failed to allocate memory for arguments",.{});
    return 1;
  };

  if (args.len < 2) {
    std.log.err("expected filename argument", .{});
    return 1; // EXIT_FAILURE
  }

  const filename: [*:0]const u8 = args[1];
  const loop = try el.WorkerEventLoop.init(allocator, filename);
  defer loop.deinit();

  _ = try loop.call_init();
  _ = loop.run(c.UV_RUN_DEFAULT);
  return 0; // EXIT_SUCCESS
}
