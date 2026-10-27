; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [18 x i8] c"Unknown command.\0A\00"
@.str.1 = private unnamed_addr constant [23 x i8] c"Processing command 2.\0A\00"
@.str.2 = private unnamed_addr constant [14 x i8] c"before break\0A\00"
@.str.3 = private unnamed_addr constant [15 x i8] c"Value is low.\0A\00"
@.str.4 = private unnamed_addr constant [16 x i8] c"Value is high.\0A\00"
@.str.5 = private unnamed_addr constant [23 x i8] c"Processing command 1.\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 1, ptr %1, align 4
  store i32 50, ptr %0, align 4
  %2 = load i32, ptr %1, align 4
  switch i32 %2, label %switch.default [
    i32 1, label %switch.case
    i32 2, label %switch.case1
  ]

switch.end:                                       ; preds = %switch.default, %switch.case1, %if.end, %if.else
  ret i32 0

switch.case:                                      ; preds = %entry
  %3 = call i32 (ptr, ...) @printf(ptr @.str.5)
  %4 = load i32, ptr %0, align 4
  %gt = icmp sgt i32 %4, 40
  br i1 %gt, label %if.then, label %if.else

switch.case1:                                     ; preds = %entry
  %5 = call i32 (ptr, ...) @printf(ptr @.str.1)
  br label %switch.end

switch.default:                                   ; preds = %entry
  %6 = call i32 (ptr, ...) @printf(ptr @.str)
  br label %switch.end

if.then:                                          ; preds = %switch.case
  %7 = call i32 (ptr, ...) @printf(ptr @.str.4)
  br label %if.end

if.end:                                           ; preds = %if.then
  %8 = call i32 (ptr, ...) @printf(ptr @.str.2)
  br label %switch.end

if.else:                                          ; preds = %switch.case
  %9 = call i32 (ptr, ...) @printf(ptr @.str.3)
  br label %switch.end
}
