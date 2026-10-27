; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str.1 = private unnamed_addr constant [4 x i8] c"%s\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %b = alloca [6 x i8], align 8
  %0 = getelementptr inbounds [6 x i8], ptr %b, i32 0, i32 0
  %a = alloca [4 x i8], align 8
  %1 = getelementptr inbounds [4 x i8], ptr %a, i32 0, i32 0
  call void @llvm.memset.p0.i64(ptr align 1 %1, i8 0, i64 4, i1 false)
  %2 = getelementptr inbounds i8, ptr %1, i32 0
  store i8 104, ptr %2, align 1
  %3 = getelementptr inbounds i8, ptr %1, i32 1
  store i8 105, ptr %3, align 1
  %4 = getelementptr inbounds i8, ptr %1, i32 2
  store i8 33, ptr %4, align 1
  %5 = getelementptr inbounds i8, ptr %1, i32 3
  store i8 0, ptr %5, align 1
  call void @llvm.memset.p0.i64(ptr align 1 %0, i8 0, i64 6, i1 false)
  %6 = getelementptr inbounds i8, ptr %0, i32 0
  store i8 72, ptr %6, align 1
  %7 = getelementptr inbounds i8, ptr %0, i32 1
  store i8 101, ptr %7, align 1
  %8 = getelementptr inbounds i8, ptr %0, i32 2
  store i8 108, ptr %8, align 1
  %9 = getelementptr inbounds i8, ptr %0, i32 3
  store i8 108, ptr %9, align 1
  %10 = getelementptr inbounds i8, ptr %0, i32 4
  store i8 111, ptr %10, align 1
  %11 = getelementptr inbounds i8, ptr %0, i32 5
  store i8 0, ptr %11, align 1
  %12 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr %1)
  %13 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr %0)
  %14 = getelementptr inbounds i8, ptr %1, i32 0
  %15 = load i8, ptr %14, align 1
  %conv = sext i8 %15 to i32
  %16 = call i32 (ptr, ...) @printf(ptr @.str, i32 %conv)
  ret i32 0
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #0

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: write) }
