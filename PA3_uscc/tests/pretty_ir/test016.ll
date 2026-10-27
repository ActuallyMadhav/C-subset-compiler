; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str.1 = private unnamed_addr constant [4 x i8] c"%d,\00"

declare i32 @printf(ptr, ...)

define void @printArray(ptr %array, i32 %size) {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 %size, ptr %1, align 4
  %2 = alloca ptr, align 8
  store ptr %array, ptr %2, align 8
  store i32 0, ptr %0, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %3 = load i32, ptr %1, align 4
  %sub = sub i32 %3, 1
  %4 = load i32, ptr %0, align 4
  %lt = icmp slt i32 %4, %sub
  br i1 %lt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %5 = load i32, ptr %0, align 4
  %6 = load ptr, ptr %2, align 8
  %7 = getelementptr inbounds i32, ptr %6, i32 %5
  %8 = load i32, ptr %7, align 4
  %9 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %8)
  %10 = load i32, ptr %0, align 4
  %inc = add i32 %10, 1
  store i32 %inc, ptr %0, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %11 = load i32, ptr %0, align 4
  %12 = load ptr, ptr %2, align 8
  %13 = getelementptr inbounds i32, ptr %12, i32 %11
  %14 = load i32, ptr %13, align 4
  %15 = call i32 (ptr, ...) @printf(ptr @.str, i32 %14)
  ret void
}

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %array = alloca [5 x i32], align 8
  %1 = getelementptr inbounds [5 x i32], ptr %array, i32 0, i32 0
  store i32 5, ptr %0, align 4
  %2 = getelementptr inbounds i32, ptr %1, i32 0
  store i32 1, ptr %2, align 4
  %3 = getelementptr inbounds i32, ptr %1, i32 1
  store i32 1, ptr %3, align 4
  %4 = getelementptr inbounds i32, ptr %1, i32 2
  store i32 2, ptr %4, align 4
  %5 = getelementptr inbounds i32, ptr %1, i32 3
  store i32 3, ptr %5, align 4
  %6 = getelementptr inbounds i32, ptr %1, i32 4
  store i32 5, ptr %6, align 4
  call void @printArray(ptr %1, i32 5)
  store i32 10, ptr %0, align 4
  %7 = load i32, ptr %0, align 4
  %8 = call i32 (ptr, ...) @printf(ptr @.str, i32 %7)
  ret i32 0
}
