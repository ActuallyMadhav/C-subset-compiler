; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [13 x i8] c"%d %d %d %d\0A\00"
@.str.1 = private unnamed_addr constant [10 x i8] c"%d %d %d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %f = alloca [4 x i32], align 8
  %0 = getelementptr inbounds [4 x i32], ptr %f, i32 0, i32 0
  %e = alloca [6 x i32], align 8
  %1 = getelementptr inbounds [6 x i32], ptr %e, i32 0, i32 0
  call void @llvm.memset.p0.i64(ptr align 1 %1, i8 0, i64 24, i1 false)
  %2 = getelementptr inbounds i32, ptr %1, i32 0
  store i32 1, ptr %2, align 4
  %3 = getelementptr inbounds i32, ptr %1, i32 1
  store i32 2, ptr %3, align 4
  %4 = getelementptr inbounds i32, ptr %1, i32 2
  store i32 3, ptr %4, align 4
  %5 = getelementptr inbounds i32, ptr %1, i32 3
  store i32 4, ptr %5, align 4
  %6 = getelementptr inbounds i32, ptr %1, i32 4
  store i32 5, ptr %6, align 4
  %7 = getelementptr inbounds i32, ptr %1, i32 5
  store i32 6, ptr %7, align 4
  call void @llvm.memset.p0.i64(ptr align 1 %0, i8 0, i64 16, i1 false)
  %8 = getelementptr inbounds i32, ptr %0, i32 0
  store i32 1, ptr %8, align 4
  %9 = getelementptr inbounds i32, ptr %0, i32 2
  store i32 3, ptr %9, align 4
  %10 = getelementptr inbounds i32, ptr %1, i32 0
  %11 = load i32, ptr %10, align 4
  %12 = getelementptr inbounds i32, ptr %1, i32 1
  %13 = load i32, ptr %12, align 4
  %14 = getelementptr inbounds i32, ptr %1, i32 2
  %15 = load i32, ptr %14, align 4
  %16 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %11, i32 %13, i32 %15)
  %17 = getelementptr inbounds i32, ptr %1, i32 3
  %18 = load i32, ptr %17, align 4
  %19 = getelementptr inbounds i32, ptr %1, i32 4
  %20 = load i32, ptr %19, align 4
  %21 = getelementptr inbounds i32, ptr %1, i32 5
  %22 = load i32, ptr %21, align 4
  %23 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %18, i32 %20, i32 %22)
  %24 = getelementptr inbounds i32, ptr %0, i32 0
  %25 = load i32, ptr %24, align 4
  %26 = getelementptr inbounds i32, ptr %0, i32 1
  %27 = load i32, ptr %26, align 4
  %28 = getelementptr inbounds i32, ptr %0, i32 2
  %29 = load i32, ptr %28, align 4
  %30 = getelementptr inbounds i32, ptr %0, i32 3
  %31 = load i32, ptr %30, align 4
  %32 = call i32 (ptr, ...) @printf(ptr @.str, i32 %25, i32 %27, i32 %29, i32 %31)
  ret i32 0
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #0

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: write) }
