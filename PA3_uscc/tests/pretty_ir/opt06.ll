; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%c\0A\00"
@.str.1 = private unnamed_addr constant [12 x i8] c"Outer loop\0A\00"
@.str.2 = private unnamed_addr constant [13 x i8] c"HELLO WORLD!\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i8, align 1
  %str = alloca [13 x i8], align 8
  %1 = getelementptr inbounds [13 x i8], ptr %str, i32 0, i32 0
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i8, align 1
  store i32 5, ptr %3, align 4
  store i32 2, ptr %2, align 4
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %1, ptr align 1 @.str.2, i64 13, i1 false)
  %5 = getelementptr inbounds i8, ptr %1, i32 0
  %6 = load i8, ptr %5, align 1
  store i8 %6, ptr %0, align 1
  br label %while.cond

while.cond:                                       ; preds = %while.end4, %entry
  %7 = load i32, ptr %3, align 4
  %gt = icmp sgt i32 %7, 0
  br i1 %gt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %8 = call i32 (ptr, ...) @printf(ptr @.str.1)
  br label %while.cond1

while.end:                                        ; preds = %while.cond
  ret i32 0

while.cond1:                                      ; preds = %while.body3, %while.body
  %9 = load i32, ptr %2, align 4
  %gt2 = icmp sgt i32 %9, 0
  br i1 %gt2, label %while.body3, label %while.end4

while.body3:                                      ; preds = %while.cond1
  %10 = load i8, ptr %0, align 1
  %conv = sext i8 %10 to i32
  %add = add i32 %conv, 32
  %conv5 = trunc i32 %add to i8
  store i8 %conv5, ptr %4, align 1
  %11 = load i8, ptr %4, align 1
  %conv6 = sext i8 %11 to i32
  %12 = call i32 (ptr, ...) @printf(ptr @.str, i32 %conv6)
  %13 = load i32, ptr %2, align 4
  %dec = sub i32 %13, 1
  store i32 %dec, ptr %2, align 4
  br label %while.cond1

while.end4:                                       ; preds = %while.cond1
  %14 = load i32, ptr %3, align 4
  %dec7 = sub i32 %14, 1
  store i32 %dec7, ptr %3, align 4
  br label %while.cond
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #0

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
