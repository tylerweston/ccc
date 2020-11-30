; ModuleID = 'ece467'
source_filename = "ece467"

define i32 @foo() {
entry:
  ret i32 2
}

define i32 @main() {
entry:
  %call_foo = call i32 @foo()
  ret i32 %call_foo
}
