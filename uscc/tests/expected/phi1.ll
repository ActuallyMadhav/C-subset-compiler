; ModuleID = 'phi1.ll'
source_filename = "phi1.ll"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str1 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %n = alloca i32, align 4
  %1 = call i32 (ptr, ...) @__isoc99_scanf(ptr @.str, ptr %n) #2
  %2 = load i32, ptr %n, align 4, !tbaa !1
  %3 = icmp slt i32 %2, 3
  br i1 %3, label %BB1, label %BB2

BB1:                                              ; preds = %BB1, %0
  br i1 %3, label %BB2, label %BB1

BB2:                                              ; preds = %BB1, %0
  ret i32 1
}

; Function Attrs: nounwind
declare i32 @__isoc99_scanf(ptr readonly captures(none), ...) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.5.0 (tags/RELEASE_350/final)"}
!1 = !{!2, !2, i64 0}
!2 = !{!"int", !3, i64 0}
!3 = !{!"omnipotent char", !4, i64 0}
!4 = !{!"Simple C/C++ TBAA"}
