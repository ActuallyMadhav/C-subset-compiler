; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [13 x i8] c"%d %d %d %d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @nine() {
entry:
  ret i32 9
}

define i32 @main() {
entry:
  %a = alloca [4 x i32], align 8
  %0 = getelementptr inbounds [4 x i32], ptr %a, i32 0, i32 0
  %1 = alloca i32, align 4
  store i32 5, ptr %1, align 4
  call void @llvm.memset.p0.i64(ptr align 1 %0, i8 0, i64 16, i1 false)
  %2 = getelementptr inbounds i32, ptr %0, i32 0
  store i32 1, ptr %2, align 4
  %3 = load i32, ptr %1, align 4
  %4 = getelementptr inbounds i32, ptr %0, i32 1
  store i32 %3, ptr %4, align 4
  %call = call i32 @nine()
  %5 = getelementptr inbounds i32, ptr %0, i32 2
  store i32 %call, ptr %5, align 4
  %6 = load i32, ptr %1, align 4
  %mul = mul i32 %6, 2
  %7 = getelementptr inbounds i32, ptr %0, i32 3
  store i32 %mul, ptr %7, align 4
  %8 = getelementptr inbounds i32, ptr %0, i32 0
  %9 = load i32, ptr %8, align 4
  %10 = getelementptr inbounds i32, ptr %0, i32 1
  %11 = load i32, ptr %10, align 4
  %12 = getelementptr inbounds i32, ptr %0, i32 2
  %13 = load i32, ptr %12, align 4
  %14 = getelementptr inbounds i32, ptr %0, i32 3
  %15 = load i32, ptr %14, align 4
  %16 = call i32 (ptr, ...) @printf(ptr @.str, i32 %9, i32 %11, i32 %13, i32 %15)
  ret i32 0
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #0

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: write) }
