; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %2, align 4
  store i32 10, ptr %1, align 4
  store i32 0, ptr %0, align 4
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %3 = load i32, ptr %1, align 4
  %gt = icmp sgt i32 %3, 0
  br i1 %gt, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %4 = load i32, ptr %1, align 4
  %dec = sub i32 %4, 1
  store i32 %dec, ptr %1, align 4
  %5 = load i32, ptr %2, align 4
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %if.then, label %if.else

while.end:                                        ; preds = %while.cond
  %7 = load i32, ptr %0, align 4
  %8 = call i32 (ptr, ...) @printf(ptr @.str, i32 %7)
  ret i32 0

if.then:                                          ; preds = %while.body
  store i32 100, ptr %0, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %if.else
  br label %while.cond

if.else:                                          ; preds = %while.body
  store i32 50, ptr %0, align 4
  br label %if.end
}
