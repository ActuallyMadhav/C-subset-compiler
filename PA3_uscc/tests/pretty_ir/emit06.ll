; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%c\0A\00"
@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i8, align 1
  %2 = alloca i32, align 4
  store i32 97, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %conv = trunc i32 %3 to i8
  store i8 %conv, ptr %1, align 1
  %4 = load i8, ptr %1, align 1
  %conv1 = sext i8 %4 to i32
  store i32 %conv1, ptr %0, align 4
  %5 = load i8, ptr %1, align 1
  %conv2 = sext i8 %5 to i32
  %6 = load i32, ptr %2, align 4
  %eq = icmp eq i32 %6, %conv2
  %7 = call i32 (ptr, ...) @printf(ptr @.str.1, i1 %eq)
  %8 = load i32, ptr %0, align 4
  %9 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %8)
  %10 = load i8, ptr %1, align 1
  %inc = add i8 %10, 1
  store i8 %inc, ptr %1, align 1
  %conv3 = sext i8 %inc to i32
  %11 = call i32 (ptr, ...) @printf(ptr @.str, i32 %conv3)
  %12 = load i8, ptr %1, align 1
  %dec = sub i8 %12, 1
  store i8 %dec, ptr %1, align 1
  %conv4 = sext i8 %dec to i32
  %13 = call i32 (ptr, ...) @printf(ptr @.str, i32 %conv4)
  ret i32 0
}
