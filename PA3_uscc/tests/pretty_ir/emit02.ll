; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [14 x i8] c"Hello world!\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @.str)
  ret i32 0
}
