; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 0, ptr %0, align 4
  br label %dowhile.body

dowhile.body:                                     ; preds = %dowhile.cond, %entry
  %2 = load i32, ptr %1, align 4
  %inc = add i32 %2, 1
  store i32 %inc, ptr %1, align 4
  %3 = load i32, ptr %0, align 4
  %add = add i32 100, %3
  store i32 %add, ptr %0, align 4
  %4 = load i32, ptr %0, align 4
  %5 = call i32 (ptr, ...) @printf(ptr @.str, i32 %4)
  br label %dowhile.cond

dowhile.cond:                                     ; preds = %dowhile.body
  %6 = load i32, ptr %1, align 4
  %lt = icmp slt i32 %6, 20
  br i1 %lt, label %dowhile.body, label %dowhile.end

dowhile.end:                                      ; preds = %dowhile.cond
  ret i32 0
}
