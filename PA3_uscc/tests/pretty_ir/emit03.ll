; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @.str, i32 5)
  %1 = call i32 (ptr, ...) @printf(ptr @.str, i32 25)
  %2 = call i32 (ptr, ...) @printf(ptr @.str, i32 1)
  %3 = call i32 (ptr, ...) @printf(ptr @.str, i32 50)
  %4 = call i32 (ptr, ...) @printf(ptr @.str, i1 false)
  %5 = call i32 (ptr, ...) @printf(ptr @.str, i1 true)
  %6 = call i32 (ptr, ...) @printf(ptr @.str, i1 false)
  %7 = call i32 (ptr, ...) @printf(ptr @.str, i1 true)
  ret i32 0
}
