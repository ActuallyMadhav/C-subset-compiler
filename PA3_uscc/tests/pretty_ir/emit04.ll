; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 5, ptr %1, align 4
  %2 = load i32, ptr %1, align 4
  %add = add i32 %2, 10
  store i32 %add, ptr %0, align 4
  %3 = load i32, ptr %1, align 4
  %4 = call i32 (ptr, ...) @printf(ptr @.str, i32 %3)
  %5 = load i32, ptr %0, align 4
  %6 = call i32 (ptr, ...) @printf(ptr @.str, i32 %5)
  ret i32 0
}
