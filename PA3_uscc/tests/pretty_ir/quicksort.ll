; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%s\0A\00"
@.str.1 = private unnamed_addr constant [36 x i8] c"thequickbrownfoxjumpsoverthelazydog\00"

declare i32 @printf(ptr, ...)

define i32 @partition(ptr %array, i32 %left, i32 %right, i32 %pivotIdx) {
entry:
  %0 = alloca i8, align 1
  %1 = alloca i32, align 4
  %2 = alloca i8, align 1
  %3 = alloca i32, align 4
  store i32 %pivotIdx, ptr %3, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 %right, ptr %5, align 4
  %6 = alloca i32, align 4
  store i32 %left, ptr %6, align 4
  %7 = alloca ptr, align 8
  store ptr %array, ptr %7, align 8
  %8 = load i32, ptr %3, align 4
  %9 = load ptr, ptr %7, align 8
  %10 = getelementptr inbounds i8, ptr %9, i32 %8
  %11 = load i8, ptr %10, align 1
  store i8 %11, ptr %2, align 1
  %12 = load i32, ptr %6, align 4
  store i32 %12, ptr %1, align 4
  %13 = load i32, ptr %6, align 4
  store i32 %13, ptr %4, align 4
  %14 = load i32, ptr %3, align 4
  %15 = load ptr, ptr %7, align 8
  %16 = getelementptr inbounds i8, ptr %15, i32 %14
  %17 = load i8, ptr %16, align 1
  store i8 %17, ptr %0, align 1
  %18 = load i32, ptr %5, align 4
  %19 = load ptr, ptr %7, align 8
  %20 = getelementptr inbounds i8, ptr %19, i32 %18
  %21 = load i8, ptr %20, align 1
  %22 = load i32, ptr %3, align 4
  %23 = load ptr, ptr %7, align 8
  %24 = getelementptr inbounds i8, ptr %23, i32 %22
  store i8 %21, ptr %24, align 1
  %25 = load i8, ptr %0, align 1
  %26 = load i32, ptr %5, align 4
  %27 = load ptr, ptr %7, align 8
  %28 = getelementptr inbounds i8, ptr %27, i32 %26
  store i8 %25, ptr %28, align 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %29 = load i32, ptr %5, align 4
  %30 = load i32, ptr %4, align 4
  %lt = icmp slt i32 %30, %29
  br i1 %lt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %31 = load i8, ptr %2, align 1
  %conv = sext i8 %31 to i32
  %32 = load i32, ptr %4, align 4
  %33 = load ptr, ptr %7, align 8
  %34 = getelementptr inbounds i8, ptr %33, i32 %32
  %35 = load i8, ptr %34, align 1
  %conv1 = sext i8 %35 to i32
  %lt2 = icmp slt i32 %conv1, %conv
  br i1 %lt2, label %if.then, label %if.end

while.end:                                        ; preds = %while.cond
  %36 = load i32, ptr %1, align 4
  %37 = load ptr, ptr %7, align 8
  %38 = getelementptr inbounds i8, ptr %37, i32 %36
  %39 = load i8, ptr %38, align 1
  store i8 %39, ptr %0, align 1
  %40 = load i32, ptr %5, align 4
  %41 = load ptr, ptr %7, align 8
  %42 = getelementptr inbounds i8, ptr %41, i32 %40
  %43 = load i8, ptr %42, align 1
  %44 = load i32, ptr %1, align 4
  %45 = load ptr, ptr %7, align 8
  %46 = getelementptr inbounds i8, ptr %45, i32 %44
  store i8 %43, ptr %46, align 1
  %47 = load i8, ptr %0, align 1
  %48 = load i32, ptr %5, align 4
  %49 = load ptr, ptr %7, align 8
  %50 = getelementptr inbounds i8, ptr %49, i32 %48
  store i8 %47, ptr %50, align 1
  %51 = load i32, ptr %1, align 4
  ret i32 %51

if.then:                                          ; preds = %while.body
  %52 = load i32, ptr %4, align 4
  %53 = load ptr, ptr %7, align 8
  %54 = getelementptr inbounds i8, ptr %53, i32 %52
  %55 = load i8, ptr %54, align 1
  store i8 %55, ptr %0, align 1
  %56 = load i32, ptr %1, align 4
  %57 = load ptr, ptr %7, align 8
  %58 = getelementptr inbounds i8, ptr %57, i32 %56
  %59 = load i8, ptr %58, align 1
  %60 = load i32, ptr %4, align 4
  %61 = load ptr, ptr %7, align 8
  %62 = getelementptr inbounds i8, ptr %61, i32 %60
  store i8 %59, ptr %62, align 1
  %63 = load i8, ptr %0, align 1
  %64 = load i32, ptr %1, align 4
  %65 = load ptr, ptr %7, align 8
  %66 = getelementptr inbounds i8, ptr %65, i32 %64
  store i8 %63, ptr %66, align 1
  %67 = load i32, ptr %1, align 4
  %inc = add i32 %67, 1
  store i32 %inc, ptr %1, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %while.body
  %68 = load i32, ptr %4, align 4
  %inc3 = add i32 %68, 1
  store i32 %inc3, ptr %4, align 4
  br label %while.cond
}

define void @quicksort(ptr %array, i32 %left, i32 %right) {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 %right, ptr %1, align 4
  %2 = alloca i32, align 4
  store i32 %left, ptr %2, align 4
  %3 = alloca ptr, align 8
  store ptr %array, ptr %3, align 8
  %4 = load i32, ptr %1, align 4
  %5 = load i32, ptr %2, align 4
  %lt = icmp slt i32 %5, %4
  br i1 %lt, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %6 = load i32, ptr %2, align 4
  %7 = load i32, ptr %1, align 4
  %sub = sub i32 %7, %6
  %div = sdiv i32 %sub, 2
  %8 = load i32, ptr %2, align 4
  %add = add i32 %8, %div
  store i32 %add, ptr %0, align 4
  %9 = load ptr, ptr %3, align 8
  %10 = load i32, ptr %2, align 4
  %11 = load i32, ptr %1, align 4
  %12 = load i32, ptr %0, align 4
  %call = call i32 @partition(ptr %9, i32 %10, i32 %11, i32 %12)
  store i32 %call, ptr %0, align 4
  %13 = load ptr, ptr %3, align 8
  %14 = load i32, ptr %2, align 4
  %15 = load i32, ptr %0, align 4
  %sub1 = sub i32 %15, 1
  call void @quicksort(ptr %13, i32 %14, i32 %sub1)
  %16 = load ptr, ptr %3, align 8
  %17 = load i32, ptr %0, align 4
  %add2 = add i32 %17, 1
  %18 = load i32, ptr %1, align 4
  call void @quicksort(ptr %16, i32 %add2, i32 %18)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  ret void
}

define i32 @main() {
entry:
  %letters = alloca [36 x i8], align 8
  %0 = getelementptr inbounds [36 x i8], ptr %letters, i32 0, i32 0
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %0, ptr align 1 @.str.1, i64 36, i1 false)
  call void @quicksort(ptr %0, i32 0, i32 34)
  %1 = call i32 (ptr, ...) @printf(ptr @.str, ptr %0)
  ret i32 0
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #0

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
