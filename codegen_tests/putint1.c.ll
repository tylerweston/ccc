; ModuleID = 'ece467'
source_filename = "ece467"

declare void @putint(i32)

define i32 @main() {
entry:
  call void @putint(i32 5)
  ret i32 100
}
