; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [12 x i8] c"5 * 6 = %d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @multiply(i32 %x, i32 %y) {
entry:
  %0 = alloca i32, align 4
  store i32 %y, ptr %0, align 4
  %1 = alloca i32, align 4
  store i32 %x, ptr %1, align 4
  %2 = load i32, ptr %0, align 4
  %3 = load i32, ptr %1, align 4
  %mul = mul i32 %3, %2
  ret i32 %mul
}

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %call = call i32 @multiply(i32 5, i32 6)
  store i32 %call, ptr %0, align 4
  %1 = load i32, ptr %0, align 4
  %2 = call i32 (ptr, ...) @printf(ptr @.str, i32 %1)
  ret i32 0
}
