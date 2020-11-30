; ModuleID = 'ece467'
source_filename = "ece467"

define i32 @foo(float %x) {
entry:
  %0 = fptosi float %x to i32
  %1 = mul i32 2, %0
  ret i32 %1
}

define i32 @main() {
entry:
  %call_foo = call i32 @foo(float 3.000000e+00)
  ret i32 %call_foo
}
