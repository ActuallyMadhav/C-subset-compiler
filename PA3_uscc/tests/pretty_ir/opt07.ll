; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %array = alloca [3 x i32], align 8
  %1 = getelementptr inbounds [3 x i32], ptr %array, i32 0, i32 0
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 10, ptr %2, align 4
  %4 = getelementptr inbounds i32, ptr %1, i32 0
  store i32 1, ptr %4, align 4
  %5 = getelementptr inbounds i32, ptr %1, i32 1
  store i32 3, ptr %5, align 4
  %6 = getelementptr inbounds i32, ptr %1, i32 2
  store i32 5, ptr %6, align 4
  %7 = getelementptr inbounds i32, ptr %1, i32 1
  %8 = load i32, ptr %7, align 4
  store i32 %8, ptr %0, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %9 = load i32, ptr %2, align 4
  %gt = icmp sgt i32 %9, 0
  br i1 %gt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %10 = load i32, ptr %0, align 4
  %add = add i32 %10, 10
  store i32 %add, ptr %3, align 4
  %11 = load i32, ptr %3, align 4
  %12 = call i32 (ptr, ...) @printf(ptr @.str, i32 %11)
  %13 = load i32, ptr %2, align 4
  %dec = sub i32 %13, 1
  store i32 %dec, ptr %2, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %14 = getelementptr inbounds i32, ptr %1, i32 2
  %15 = load i32, ptr %14, align 4
  %16 = call i32 (ptr, ...) @printf(ptr @.str, i32 %15)
  ret i32 0
}
