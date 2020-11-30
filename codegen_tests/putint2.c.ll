; ModuleID = 'ece467'
source_filename = "ece467"

declare void @putint(i32)

define float @foo(float %a, float %b) {
entry:
  %0 = fmul float %a, %a
  %1 = fmul float %b, %b
  %2 = fadd float %0, %1
  ret float %2
}

define i32 @main() {
entry:
  %0 = call float @foo(float 2.000000e+00, float 4.000000e+00)
  %1 = fdiv float %0, 2.000000e+00
  %2 = fptosi float %1 to i32
  %3 = sub i32 %2, 2
  call void @putint(i32 %3)
  ret i32 0
}
