; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [25 x i8] c"%d is a multiple of %d.\0A\00"
@.str.1 = private unnamed_addr constant [34 x i8] c" is not a multiple of 2, 3 or 5.\0A\00"
@.str.2 = private unnamed_addr constant [23 x i8] c" is a multiple of %d.\0A\00"
@.str.3 = private unnamed_addr constant [3 x i8] c"%d\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  store i32 1, ptr %0, align 4
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %1 = load i32, ptr %0, align 4
  %lt = icmp slt i32 %1, 11
  br i1 %lt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %2 = load i32, ptr %0, align 4
  %3 = call i32 (ptr, ...) @printf(ptr @.str.3, i32 %2)
  %4 = load i32, ptr %0, align 4
  %mod = srem i32 %4, 2
  %5 = icmp eq i32 %mod, 0
  %6 = zext i1 %5 to i32
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %if.then, label %if.else

while.end:                                        ; preds = %while.cond
  ret i32 0

if.then:                                          ; preds = %while.body
  %8 = call i32 (ptr, ...) @printf(ptr @.str.2, i32 2)
  %9 = load i32, ptr %0, align 4
  %mod9 = srem i32 %9, 3
  %10 = icmp eq i32 %mod9, 0
  %11 = zext i1 %10 to i32
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %if.then10, label %if.end11

if.end:                                           ; preds = %if.end14, %if.end3
  %13 = load i32, ptr %0, align 4
  %inc = add i32 %13, 1
  store i32 %inc, ptr %0, align 4
  br label %while.cond

if.else:                                          ; preds = %while.body
  %14 = load i32, ptr %0, align 4
  %mod1 = srem i32 %14, 3
  %15 = icmp eq i32 %mod1, 0
  %16 = zext i1 %15 to i32
  %17 = icmp ne i32 %16, 0
  br i1 %17, label %if.then2, label %if.else4

if.then2:                                         ; preds = %if.else
  %18 = call i32 (ptr, ...) @printf(ptr @.str.2, i32 3)
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end7
  br label %if.end

if.else4:                                         ; preds = %if.else
  %19 = load i32, ptr %0, align 4
  %mod5 = srem i32 %19, 5
  %20 = icmp eq i32 %mod5, 0
  %21 = zext i1 %20 to i32
  %22 = icmp ne i32 %21, 0
  br i1 %22, label %if.then6, label %if.else8

if.then6:                                         ; preds = %if.else4
  %23 = call i32 (ptr, ...) @printf(ptr @.str.2, i32 5)
  br label %if.end7

if.end7:                                          ; preds = %if.then6, %if.else8
  br label %if.end3

if.else8:                                         ; preds = %if.else4
  %24 = call i32 (ptr, ...) @printf(ptr @.str.1)
  br label %if.end7

if.then10:                                        ; preds = %if.then
  %25 = load i32, ptr %0, align 4
  %26 = call i32 (ptr, ...) @printf(ptr @.str, i32 %25, i32 3)
  br label %if.end11

if.end11:                                         ; preds = %if.then10, %if.then
  %27 = load i32, ptr %0, align 4
  %mod12 = srem i32 %27, 5
  %28 = icmp eq i32 %mod12, 0
  %29 = zext i1 %28 to i32
  %30 = icmp ne i32 %29, 0
  br i1 %30, label %if.then13, label %if.end14

if.then13:                                        ; preds = %if.end11
  %31 = load i32, ptr %0, align 4
  %32 = call i32 (ptr, ...) @printf(ptr @.str, i32 %31, i32 5)
  br label %if.end14

if.end14:                                         ; preds = %if.then13, %if.end11
  br label %if.end
}
