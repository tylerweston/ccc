; ModuleID = 'ece467'
source_filename = "ece467"

define i32 @id(i32 %x) {
entry:
  ret i32 %x
}

define i32 @main() {
entry:
  %call_id = call i32 @id(i32 3)
  ret i32 %call_id
}
