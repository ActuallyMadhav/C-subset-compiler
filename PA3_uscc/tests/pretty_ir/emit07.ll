; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 5, ptr %0, align 4
  %2 = load i32, ptr %0, align 4
  %3 = icmp eq i32 %2, 0
  %4 = zext i1 %3 to i32
  %5 = call i32 (ptr, ...) @printf(ptr @.str, i32 %4)
  br label %while.cond

while.cond:                                       ; preds = %while.end3, %entry
  %6 = load i32, ptr %0, align 4
  %gt = icmp sgt i32 %6, 0
  br i1 %gt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  store i32 8, ptr %1, align 4
  %7 = load i32, ptr %0, align 4
  %8 = call i32 (ptr, ...) @printf(ptr @.str, i32 %7)
  %9 = load i32, ptr %0, align 4
  %dec = sub i32 %9, 1
  store i32 %dec, ptr %0, align 4
  br label %while.cond1

while.end:                                        ; preds = %while.cond
  %10 = load i32, ptr %0, align 4
  %11 = icmp eq i32 %10, 0
  %12 = zext i1 %11 to i32
  %13 = call i32 (ptr, ...) @printf(ptr @.str, i32 %12)
  ret i32 0

while.cond1:                                      ; preds = %while.body2, %while.body
  %14 = load i32, ptr %1, align 4
  %lt = icmp slt i32 %14, 10
  br i1 %lt, label %while.body2, label %while.end3

while.body2:                                      ; preds = %while.cond1
  %15 = load i32, ptr %1, align 4
  %16 = call i32 (ptr, ...) @printf(ptr @.str, i32 %15)
  %17 = load i32, ptr %1, align 4
  %inc = add i32 %17, 1
  store i32 %inc, ptr %1, align 4
  br label %while.cond1

while.end3:                                       ; preds = %while.cond1
  br label %while.cond
}
