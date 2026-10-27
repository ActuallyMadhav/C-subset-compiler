; ModuleID = 'main'
source_filename = "main"

@.str = private unnamed_addr constant [19 x i8] c"It's a consonant.\0A\00"
@.str.1 = private unnamed_addr constant [15 x i8] c"It's a vowel.\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = alloca i8, align 1
  store i8 105, ptr %0, align 1
  %1 = load i8, ptr %0, align 1
  %conv = sext i8 %1 to i32
  switch i32 %conv, label %switch.default [
    i32 97, label %switch.case
    i32 101, label %switch.case1
    i32 105, label %switch.case2
    i32 111, label %switch.case3
    i32 117, label %switch.case4
  ]

switch.end:                                       ; preds = %switch.default, %switch.case4
  ret i32 0

switch.case:                                      ; preds = %entry
  br label %switch.case1

switch.case1:                                     ; preds = %switch.case, %entry
  br label %switch.case2

switch.case2:                                     ; preds = %switch.case1, %entry
  br label %switch.case3

switch.case3:                                     ; preds = %switch.case2, %entry
  br label %switch.case4

switch.case4:                                     ; preds = %switch.case3, %entry
  %2 = call i32 (ptr, ...) @printf(ptr @.str.1)
  br label %switch.end

switch.default:                                   ; preds = %entry
  %3 = call i32 (ptr, ...) @printf(ptr @.str)
  br label %switch.end
}
