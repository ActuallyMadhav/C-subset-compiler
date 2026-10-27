; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @sum3(ptr %x) {
entry:
  %0 = alloca ptr, align 8
  store ptr %x, ptr %0, align 8
  %1 = load ptr, ptr %0, align 8
  %2 = getelementptr inbounds i32, ptr %1, i32 2
  %3 = load i32, ptr %2, align 4
  %4 = load ptr, ptr %0, align 8
  %5 = getelementptr inbounds i32, ptr %4, i32 1
  %6 = load i32, ptr %5, align 4
  %7 = load ptr, ptr %0, align 8
  %8 = getelementptr inbounds i32, ptr %7, i32 0
  %9 = load i32, ptr %8, align 4
  %add = add i32 %9, %6
  %add1 = add i32 %add, %3
  ret i32 %add1
}

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
  %4 = getelementptr inbounds i32, ptr %2, i32 4
  %call = call i32 @sum3(ptr %4)
  %5 = call i32 (ptr, ...) @printf(ptr @.str, i32 %call)
  %6 = getelementptr inbounds i32, ptr %2, i32 9
  %call8 = call i32 @sum3(ptr %6)
  %7 = call i32 (ptr, ...) @printf(ptr @.str, i32 %call8)
  ret i32 0

while.cond1:                                      ; preds = %while.body3, %while.body
  %8 = load i32, ptr %0, align 4
  %lt2 = icmp slt i32 %8, 4
  br i1 %lt2, label %while.body3, label %while.end4

while.body3:                                      ; preds = %while.cond1
  %9 = load i32, ptr %0, align 4
  %10 = load i32, ptr %1, align 4
  %mul = mul i32 %10, 10
  %add = add i32 %mul, %9
  %11 = load i32, ptr %1, align 4
  %12 = load i32, ptr %0, align 4
  %idx = mul i32 %11, 4
  %idx5 = add i32 %idx, %12
  %13 = getelementptr inbounds i32, ptr %2, i32 %idx5
  store i32 %add, ptr %13, align 4
  %14 = load i32, ptr %0, align 4
  %add6 = add i32 %14, 1
  store i32 %add6, ptr %0, align 4
  br label %while.cond1

while.end4:                                       ; preds = %while.cond1
  %15 = load i32, ptr %1, align 4
  %add7 = add i32 %15, 1
  store i32 %add7, ptr %1, align 4
  br label %while.cond
}
