; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str.1 = private unnamed_addr constant [4 x i8] c"%d \00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  %a = alloca [12 x i32], align 8
  %2 = getelementptr inbounds [12 x i32], ptr %a, i32 0, i32 0
  store i32 0, ptr %1, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.end4, %entry
  %3 = load i32, ptr %1, align 4
  %lt = icmp slt i32 %3, 3
  br i1 %lt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  store i32 0, ptr %0, align 4
  br label %while.cond1

while.end:                                        ; preds = %while.cond
  store i32 0, ptr %1, align 4
  br label %while.cond8

while.cond1:                                      ; preds = %while.body3, %while.body
  %4 = load i32, ptr %0, align 4
  %lt2 = icmp slt i32 %4, 4
  br i1 %lt2, label %while.body3, label %while.end4

while.body3:                                      ; preds = %while.cond1
  %5 = load i32, ptr %0, align 4
  %6 = load i32, ptr %1, align 4
  %mul = mul i32 %6, 10
  %add = add i32 %mul, %5
  %7 = load i32, ptr %1, align 4
  %8 = load i32, ptr %0, align 4
  %idx = mul i32 %7, 4
  %idx5 = add i32 %idx, %8
  %9 = getelementptr inbounds i32, ptr %2, i32 %idx5
  store i32 %add, ptr %9, align 4
  %10 = load i32, ptr %0, align 4
  %add6 = add i32 %10, 1
  store i32 %add6, ptr %0, align 4
  br label %while.cond1

while.end4:                                       ; preds = %while.cond1
  %11 = load i32, ptr %1, align 4
  %add7 = add i32 %11, 1
  store i32 %add7, ptr %1, align 4
  br label %while.cond

while.cond8:                                      ; preds = %while.end15, %while.end
  %12 = load i32, ptr %1, align 4
  %lt9 = icmp slt i32 %12, 3
  br i1 %lt9, label %while.body10, label %while.end11

while.body10:                                     ; preds = %while.cond8
  store i32 0, ptr %0, align 4
  br label %while.cond12

while.end11:                                      ; preds = %while.cond8
  ret i32 0

while.cond12:                                     ; preds = %while.body14, %while.body10
  %13 = load i32, ptr %0, align 4
  %lt13 = icmp slt i32 %13, 4
  br i1 %lt13, label %while.body14, label %while.end15

while.body14:                                     ; preds = %while.cond12
  %14 = load i32, ptr %1, align 4
  %15 = load i32, ptr %0, align 4
  %idx16 = mul i32 %14, 4
  %idx17 = add i32 %idx16, %15
  %16 = getelementptr inbounds i32, ptr %2, i32 %idx17
  %17 = load i32, ptr %16, align 4
  %18 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %17)
  %19 = load i32, ptr %0, align 4
  %add18 = add i32 %19, 1
  store i32 %add18, ptr %0, align 4
  br label %while.cond12

while.end15:                                      ; preds = %while.cond12
  %20 = call i32 (ptr, ...) @printf(ptr @.str)
  %21 = load i32, ptr %1, align 4
  %add19 = add i32 %21, 1
  store i32 %add19, ptr %1, align 4
  br label %while.cond8
}
