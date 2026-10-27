; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str.1 = private unnamed_addr constant [6 x i8] c"%c%c\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %g = alloca [6 x i8], align 8
  %1 = getelementptr inbounds [6 x i8], ptr %g, i32 0, i32 0
  call void @llvm.memset.p0.i64(ptr align 1 %1, i8 0, i64 6, i1 false)
  %2 = getelementptr inbounds i8, ptr %1, i32 0
  store i8 97, ptr %2, align 1
  %3 = getelementptr inbounds i8, ptr %1, i32 1
  store i8 98, ptr %3, align 1
  %4 = getelementptr inbounds i8, ptr %1, i32 2
  store i8 0, ptr %4, align 1
  %5 = getelementptr inbounds i8, ptr %1, i32 3
  store i8 99, ptr %5, align 1
  %6 = getelementptr inbounds i8, ptr %1, i32 4
  store i8 100, ptr %6, align 1
  %7 = getelementptr inbounds i8, ptr %1, i32 5
  store i8 0, ptr %7, align 1
  store i32 0, ptr %0, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %8 = load i32, ptr %0, align 4
  %lt = icmp slt i32 %8, 2
  br i1 %lt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %9 = load i32, ptr %0, align 4
  %idx = mul i32 %9, 3
  %idx1 = add i32 %idx, 0
  %10 = getelementptr inbounds i8, ptr %1, i32 %idx1
  %11 = load i8, ptr %10, align 1
  %conv = sext i8 %11 to i32
  %12 = load i32, ptr %0, align 4
  %idx2 = mul i32 %12, 3
  %idx3 = add i32 %idx2, 1
  %13 = getelementptr inbounds i8, ptr %1, i32 %idx3
  %14 = load i8, ptr %13, align 1
  %conv4 = sext i8 %14 to i32
  %15 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %conv, i32 %conv4)
  %16 = load i32, ptr %0, align 4
  %add = add i32 %16, 1
  store i32 %add, ptr %0, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %17 = getelementptr inbounds i8, ptr %1, i32 4
  %18 = load i8, ptr %17, align 1
  %conv5 = sext i8 %18 to i32
  %19 = call i32 (ptr, ...) @printf(ptr @.str, i32 %conv5)
  ret i32 0
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #0

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: write) }
