; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [10 x i8] c"%d %d %d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %b = alloca [8 x i32], align 8
  %3 = getelementptr inbounds [8 x i32], ptr %b, i32 0, i32 0
  %a = alloca [24 x i32], align 8
  %4 = getelementptr inbounds [24 x i32], ptr %a, i32 0, i32 0
  call void @llvm.memset.p0.i64(ptr align 1 %3, i8 0, i64 32, i1 false)
  %5 = getelementptr inbounds i32, ptr %3, i32 0
  store i32 1, ptr %5, align 4
  %6 = getelementptr inbounds i32, ptr %3, i32 1
  store i32 2, ptr %6, align 4
  %7 = getelementptr inbounds i32, ptr %3, i32 2
  store i32 3, ptr %7, align 4
  %8 = getelementptr inbounds i32, ptr %3, i32 3
  store i32 4, ptr %8, align 4
  %9 = getelementptr inbounds i32, ptr %3, i32 4
  store i32 5, ptr %9, align 4
  %10 = getelementptr inbounds i32, ptr %3, i32 5
  store i32 6, ptr %10, align 4
  %11 = getelementptr inbounds i32, ptr %3, i32 6
  store i32 7, ptr %11, align 4
  %12 = getelementptr inbounds i32, ptr %3, i32 7
  store i32 8, ptr %12, align 4
  store i32 0, ptr %1, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.end4, %entry
  %13 = load i32, ptr %1, align 4
  %lt = icmp slt i32 %13, 2
  br i1 %lt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  store i32 0, ptr %0, align 4
  br label %while.cond1

while.end:                                        ; preds = %while.cond
  %14 = getelementptr inbounds i32, ptr %4, i32 0
  %15 = load i32, ptr %14, align 4
  %16 = getelementptr inbounds i32, ptr %4, i32 23
  %17 = load i32, ptr %16, align 4
  %18 = getelementptr inbounds i32, ptr %4, i32 6
  %19 = load i32, ptr %18, align 4
  %20 = call i32 (ptr, ...) @printf(ptr @.str, i32 %15, i32 %17, i32 %19)
  %21 = getelementptr inbounds i32, ptr %3, i32 0
  %22 = load i32, ptr %21, align 4
  %23 = getelementptr inbounds i32, ptr %3, i32 5
  %24 = load i32, ptr %23, align 4
  %25 = getelementptr inbounds i32, ptr %3, i32 7
  %26 = load i32, ptr %25, align 4
  %27 = call i32 (ptr, ...) @printf(ptr @.str, i32 %22, i32 %24, i32 %26)
  ret i32 0

while.cond1:                                      ; preds = %while.end8, %while.body
  %28 = load i32, ptr %0, align 4
  %lt2 = icmp slt i32 %28, 3
  br i1 %lt2, label %while.body3, label %while.end4

while.body3:                                      ; preds = %while.cond1
  store i32 0, ptr %2, align 4
  br label %while.cond5

while.end4:                                       ; preds = %while.cond1
  %29 = load i32, ptr %1, align 4
  %add16 = add i32 %29, 1
  store i32 %add16, ptr %1, align 4
  br label %while.cond

while.cond5:                                      ; preds = %while.body7, %while.body3
  %30 = load i32, ptr %2, align 4
  %lt6 = icmp slt i32 %30, 4
  br i1 %lt6, label %while.body7, label %while.end8

while.body7:                                      ; preds = %while.cond5
  %31 = load i32, ptr %2, align 4
  %32 = load i32, ptr %0, align 4
  %mul = mul i32 %32, 10
  %33 = load i32, ptr %1, align 4
  %mul9 = mul i32 %33, 100
  %add = add i32 %mul9, %mul
  %add10 = add i32 %add, %31
  %34 = load i32, ptr %1, align 4
  %35 = load i32, ptr %0, align 4
  %idx = mul i32 %34, 3
  %idx11 = add i32 %idx, %35
  %36 = load i32, ptr %2, align 4
  %idx12 = mul i32 %idx11, 4
  %idx13 = add i32 %idx12, %36
  %37 = getelementptr inbounds i32, ptr %4, i32 %idx13
  store i32 %add10, ptr %37, align 4
  %38 = load i32, ptr %2, align 4
  %add14 = add i32 %38, 1
  store i32 %add14, ptr %2, align 4
  br label %while.cond5

while.end8:                                       ; preds = %while.cond5
  %39 = load i32, ptr %0, align 4
  %add15 = add i32 %39, 1
  store i32 %add15, ptr %0, align 4
  br label %while.cond1
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #0

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: write) }
