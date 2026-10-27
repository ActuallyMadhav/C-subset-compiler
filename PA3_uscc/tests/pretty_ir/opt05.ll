; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%c\0A\00"
@.str.1 = private unnamed_addr constant [13 x i8] c"HELLO WORLD!\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i8, align 1
  %str = alloca [13 x i8], align 8
  %1 = getelementptr inbounds [13 x i8], ptr %str, i32 0, i32 0
  %2 = alloca i32, align 4
  %3 = alloca i8, align 1
  store i32 10, ptr %2, align 4
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %1, ptr align 1 @.str.1, i64 13, i1 false)
  %4 = getelementptr inbounds i8, ptr %1, i32 1
  %5 = load i8, ptr %4, align 1
  store i8 %5, ptr %0, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %6 = load i32, ptr %2, align 4
  %gt = icmp sgt i32 %6, 0
  br i1 %gt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %7 = load i8, ptr %0, align 1
  %conv = sext i8 %7 to i32
  %add = add i32 %conv, 32
  %conv1 = trunc i32 %add to i8
  store i8 %conv1, ptr %3, align 1
  %8 = load i8, ptr %3, align 1
  %conv2 = sext i8 %8 to i32
  %9 = call i32 (ptr, ...) @printf(ptr @.str, i32 %conv2)
  %10 = load i32, ptr %2, align 4
  %dec = sub i32 %10, 1
  store i32 %dec, ptr %2, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  ret i32 0
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #0

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
