; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [10 x i8] c"continue\0A\00"
@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str.2 = private unnamed_addr constant [7 x i8] c"break\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 0, ptr %0, align 4
  br label %dowhile.body

dowhile.body:                                     ; preds = %dowhile.cond, %entry
  %2 = load i32, ptr %1, align 4
  %eq = icmp eq i32 %2, 15
  br i1 %eq, label %if.then, label %if.end

dowhile.cond:                                     ; preds = %if.end3, %if.else
  %3 = load i32, ptr %1, align 4
  %lt = icmp slt i32 %3, 25
  br i1 %lt, label %dowhile.body, label %dowhile.end

dowhile.end:                                      ; preds = %dowhile.cond, %if.then
  ret i32 0

if.then:                                          ; preds = %dowhile.body
  %4 = call i32 (ptr, ...) @printf(ptr @.str.2)
  br label %dowhile.end

if.end:                                           ; preds = %dowhile.body
  %5 = load i32, ptr %1, align 4
  %inc = add i32 %5, 1
  store i32 %inc, ptr %1, align 4
  %6 = load i32, ptr %1, align 4
  %eq1 = icmp eq i32 %6, 5
  br i1 %eq1, label %if.then2, label %if.else

if.then2:                                         ; preds = %if.end
  %7 = load i32, ptr %0, align 4
  %add = add i32 100, %7
  store i32 %add, ptr %0, align 4
  %8 = load i32, ptr %0, align 4
  %9 = call i32 (ptr, ...) @printf(ptr @.str.1, i32 %8)
  br label %if.end3

if.end3:                                          ; preds = %if.then2
  br label %dowhile.cond

if.else:                                          ; preds = %if.end
  %10 = call i32 (ptr, ...) @printf(ptr @.str)
  br label %dowhile.cond
}
