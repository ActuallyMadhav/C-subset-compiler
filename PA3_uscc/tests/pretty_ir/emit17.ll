; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [10 x i8] c"%d %d %d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %e = alloca [6 x i32], align 8
  %0 = getelementptr inbounds [6 x i32], ptr %e, i32 0, i32 0
  call void @llvm.memset.p0.i64(ptr align 1 %0, i8 0, i64 24, i1 false)
  %1 = getelementptr inbounds i32, ptr %0, i32 0
  store i32 1, ptr %1, align 4
  %2 = getelementptr inbounds i32, ptr %0, i32 1
  store i32 2, ptr %2, align 4
  %3 = getelementptr inbounds i32, ptr %0, i32 2
  store i32 3, ptr %3, align 4
  %4 = getelementptr inbounds i32, ptr %0, i32 3
  store i32 4, ptr %4, align 4
  %5 = getelementptr inbounds i32, ptr %0, i32 4
  store i32 5, ptr %5, align 4
  %6 = getelementptr inbounds i32, ptr %0, i32 5
  store i32 6, ptr %6, align 4
  %7 = getelementptr inbounds i32, ptr %0, i32 0
  %8 = load i32, ptr %7, align 4
  %9 = getelementptr inbounds i32, ptr %0, i32 1
  %10 = load i32, ptr %9, align 4
  %11 = getelementptr inbounds i32, ptr %0, i32 2
  %12 = load i32, ptr %11, align 4
  %13 = call i32 (ptr, ...) @printf(ptr @.str, i32 %8, i32 %10, i32 %12)
  %14 = getelementptr inbounds i32, ptr %0, i32 3
  %15 = load i32, ptr %14, align 4
  %16 = getelementptr inbounds i32, ptr %0, i32 4
  %17 = load i32, ptr %16, align 4
  %18 = getelementptr inbounds i32, ptr %0, i32 5
  %19 = load i32, ptr %18, align 4
  %20 = call i32 (ptr, ...) @printf(ptr @.str, i32 %15, i32 %17, i32 %19)
  ret i32 0
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #0

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: write) }
