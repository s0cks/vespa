const sdk = @import("vespa-wasm-zig-sdk");

export fn init() callconv(.c) bool {
  const text = "Cross-library payload";
  sdk.VespaPrint(text.ptr, @intCast(text.len));
  return true;
}

export fn view() callconv(.c) bool {
  const text = "zig view";
  sdk.VespaPrint(text.ptr, @intCast(text.len));
  return true;
}

export fn update() callconv(.c) bool {
  const text = "zig update";
  sdk.VespaPrint(text.ptr, @intCast(text.len));
  return true;
}
