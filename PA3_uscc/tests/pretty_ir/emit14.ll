; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str.1 = private unnamed_addr constant [3 x i8] c"%d\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %b = alloca [3 x i32], align 8
  %1 = getelementptr inbounds [3 x i32], ptr %b, i32 0, i32 0
  %a = alloca [5 x i32], align 8
  %2 = getelementptr inbounds [5 x i32], ptr %a, i32 0, i32 0
  call void @llvm.memset.p0.i64(ptr align 1 %2, i8 0, i64 20, i1 false)
  %3 = getelementptr inbounds i32, ptr %2, i32 0
  store i32 1, ptr %3, align 4
  %4 = getelementptr inbounds i32, ptr %2, i32 1
  store i32 2, ptr %4, align 4
  call void @llvm.memset.p0.i64(ptr align 1 %1, i8 0, i64 12, i1 false)
  store i32 0, ptr %0, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %5 = load i32, ptr %0, align 4
  %lt = icmp slt i32 %5, 5
  br i1 %lt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %6 = load i32, ptr %0, align 4
  %7 = getelementptr inbounds i32, ptr %2, i32 %6
  %8 = load i32, ptr %7, align 4
  %9 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %8)
  %10 = load i32, ptr %0, align 4
  %add = add i32 %10, 1
  store i32 %add, ptr %0, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %11 = call i32 (ptr, ...) @printf(ptr @.str)
  store i32 0, ptr %0, align 4
  br label %while.cond1

while.cond1:                                      ; preds = %while.body3, %while.end
  %12 = load i32, ptr %0, align 4
  %lt2 = icmp slt i32 %12, 3
  br i1 %lt2, label %while.body3, label %while.end4

while.body3:                                      ; preds = %while.cond1
  %13 = load i32, ptr %0, align 4
  %14 = getelementptr inbounds i32, ptr %1, i32 %13
  %15 = load i32, ptr %14, align 4
  %16 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %15)
  %17 = load i32, ptr %0, align 4
  %add5 = add i32 %17, 1
  store i32 %add5, ptr %0, align 4
  br label %while.cond1

while.end4:                                       ; preds = %while.cond1
  %18 = call i32 (ptr, ...) @printf(ptr @.str)
  ret i32 0
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #0

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: write) }
