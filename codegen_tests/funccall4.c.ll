; ModuleID = 'ece467'
source_filename = "ece467"

define i32 @foo(i32 %a, i32 %b) {
entry:
  %0 = mul i32 %a, %a
  %1 = mul i32 %b, %b
  %2 = add i32 %0, %1
  ret i32 %2
}

define i32 @main() {
entry:
  %call_foo = call i32 @foo(i32 2, i32 3)
  ret i32 %call_foo
}
