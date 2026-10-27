; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [6 x i8] c"last\0A\00"
@.str.1 = private unnamed_addr constant [11 x i8] c"continue2\0A\00"
@.str.2 = private unnamed_addr constant [7 x i8] c"break\0A\00"
@.str.3 = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str.4 = private unnamed_addr constant [10 x i8] c"continue\0A\00"

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
  %inc = add i32 %2, 1
  store i32 %inc, ptr %1, align 4
  %3 = load i32, ptr %1, align 4
  %eq = icmp eq i32 %3, 5
  br i1 %eq, label %if.then, label %if.end

dowhile.cond:                                     ; preds = %if.end3, %if.else, %if.then
  %4 = load i32, ptr %1, align 4
  %lt = icmp slt i32 %4, 25
  br i1 %lt, label %dowhile.body, label %dowhile.end

dowhile.end:                                      ; preds = %dowhile.cond, %if.then2
  ret i32 0

if.then:                                          ; preds = %dowhile.body
  %5 = call i32 (ptr, ...) @printf(ptr @.str.4)
  br label %dowhile.cond

if.end:                                           ; preds = %dowhile.body
  %6 = load i32, ptr %0, align 4
  %add = add i32 100, %6
  store i32 %add, ptr %0, align 4
  %7 = load i32, ptr %0, align 4
  %8 = call i32 (ptr, ...) @printf(ptr @.str.3, i32 %7)
  %9 = load i32, ptr %1, align 4
  %eq1 = icmp eq i32 %9, 15
  br i1 %eq1, label %if.then2, label %if.else

if.then2:                                         ; preds = %if.end
  %10 = call i32 (ptr, ...) @printf(ptr @.str.2)
  br label %dowhile.end

if.end3:                                          ; No predecessors!
  %11 = call i32 (ptr, ...) @printf(ptr @.str)
  br label %dowhile.cond

if.else:                                          ; preds = %if.end
  %12 = call i32 (ptr, ...) @printf(ptr @.str.1)
  br label %dowhile.cond
}
