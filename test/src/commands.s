mov %al,%al
mov %al,%ah
mov %al,%ch
mov %ax,%ax
mov %ax,%sp
mov %ax,%bp
mov %eax,%eax
mov %eax,%esp
mov %eax,%ebp
mov %rax,%rax
mov %rax,%rsp
mov %rax,%rbp
mov %rax,%r8
mov %rax,%r12
mov %rax,%r13
nop
mov %eax,(%eax)
mov %eax,(%esp)
mov %eax,(%ebp)
mov %eax,(%rax)
mov %eax,(%rsp)
mov %eax,(%rbp)
mov %eax,(%r8)
mov %eax,(%r12)
mov %eax,(%r13)
mov %eax,0x7f(%eax)
mov %eax,0x7f(%esp)
mov %eax,0x7f(%ebp)
mov %eax,0x7f(%rax)
mov %eax,0x7f(%rsp)
mov %eax,0x7f(%rbp)
mov %eax,0x7f(%r8)
mov %eax,0x7f(%r12)
mov %eax,0x7f(%r13)
mov %eax,0xff(%eax)
mov %eax,0xff(%esp)
mov %eax,0xff(%ebp)
mov %eax,0xff(%rax)
mov %eax,0xff(%rsp)
mov %eax,0xff(%rbp)
mov %eax,0xff(%r8)
mov %eax,0xff(%r12)
mov %eax,0xff(%r13)
mov %eax,(%eax,%eax,2)
mov %eax,(%eax,%ebp,2)
mov %eax,(%rax,%rax,2)
mov %eax,(%rax,%rbp,2)
mov %eax,(%rax,%r8,2)
mov %eax,(%rax,%r12,2)
mov %eax,(%rax,%r13,2)
mov %eax,0x7f(%eax,%eax,2)
mov %eax,0x7f(%eax,%ebp,2)
mov %eax,0x7f(%rax,%rax,2)
mov %eax,0x7f(%rax,%rbp,2)
mov %eax,0x7f(%rax,%r8,2)
mov %eax,0x7f(%rax,%r12,2)
mov %eax,0x7f(%rax,%r13,2)
mov %eax,0xff(%eax,%eax,2)
mov %eax,0xff(%eax,%ebp,2)
mov %eax,0xff(%rax,%rax,2)
mov %eax,0xff(%rax,%rbp,2)
mov %eax,0xff(%rax,%r8,2)
mov %eax,0xff(%rax,%r12,2)
mov %eax,0xff(%rax,%r13,2)
mov %eax,(%esp,%eax,2)
mov %eax,(%ebp,%eax,2)
mov %eax,(%rsp,%rax,2)
mov %eax,(%rbp,%rax,2)
mov %eax,(%r8,%rax,2)
mov %eax,(%r12,%rax,2)
mov %eax,(%r13,%rax,2)
mov %eax,0x7f(%esp,%eax,2)
mov %eax,0x7f(%ebp,%eax,2)
mov %eax,0x7f(%rsp,%rax,2)
mov %eax,0x7f(%rbp,%rax,2)
mov %eax,0x7f(%r8,%rax,2)
mov %eax,0x7f(%r12,%rax,2)
mov %eax,0x7f(%r13,%rax,2)
mov %eax,0xff(%esp,%eax,2)
mov %eax,0xff(%ebp,%eax,2)
mov %eax,0xff(%rsp,%rax,2)
mov %eax,0xff(%rbp,%rax,2)
mov %eax,0xff(%r8,%rax,2)
mov %eax,0xff(%r12,%rax,2)
mov %eax,0xff(%r13,%rax,2)
mov %eax,0x7f(,%eax,2)
mov %eax,0x7f(,%ebp,2)
mov %eax,0x7f(,%rax,2)
mov %eax,0x7f(,%rbp,2)
mov %eax,0x7f(,%r8,2)
mov %eax,0x7f(,%r12,2)
mov %eax,0x7f(,%r13,2)
nop
mov %al,0x7fffffff
mov %ah,0x7fffffff
mov %ch,0x7fffffff
mov %ax,0x7fffffff
mov %sp,0x7fffffff
mov %bp,0x7fffffff
mov %eax,0x7fffffff
mov %esp,0x7fffffff
mov %ebp,0x7fffffff
mov %rax,0x7fffffff
mov %rsp,0x7fffffff
mov %rbp,0x7fffffff
mov %r8,0x7fffffff
mov %r12,0x7fffffff
mov %r13,0x7fffffff
nop
mov $0xff,%cl
mov $0xff,%cx
mov $0xff,%ecx
mov $0xff,%rcx
movb $0xff,(%eax)
movw $0xff,(%eax)
movl $0xff,(%eax)
movq $0xff,(%eax)
movb $0xff,0x7fffffff
movw $0xff,0x7fffffff
movl $0xff,0x7fffffff
movq $0xff,0x7fffffff
mov %cl,(%eax)
mov %cx,(%eax)
mov %ecx,(%eax)
mov %rcx,(%eax)
mov (%eax),%cl
mov (%eax),%cx
mov (%eax),%ecx
mov (%eax),%rcx
mov %cl,0x7fffffff
mov %cx,0x7fffffff
mov %ecx,0x7fffffff
mov %rcx,0x7fffffff
mov 0x7fffffff,%cl
mov 0x7fffffff,%cx
mov 0x7fffffff,%ecx
mov 0x7fffffff,%rcx
mov %al,0xffffffff
mov %ax,0xffffffff
mov %eax,0xffffffff
mov %rax,0xffffffff
mov 0xffffffff,%al
mov 0xffffffff,%ax
mov 0xffffffff,%eax
mov 0xffffffff,%rax
nop
callw 0
callw *0x7fffffff
callw *(%eax)
call 0
call *0x7fffffff
call *(%eax)
lcallw *0x7fffffff
lcallw *(%eax)
lcall *0x7fffffff
lcall *(%eax)
nop
enterw $0xeeee, $0xff
enter $0xeeee, $0xff
leavew
leave
nop
lea (%eax),%cx
lea (%eax),%ecx
lea (%eax),%rcx
nop
add $0xff,%al
add $0xff,%cl
add $0xff,%ax
add $0xff,%cx
add $0xff,%eax
add $0xff,%ecx
add $0xff,%rax
add $0xff,%rcx
addb $0xff,(%eax)
addw $0xff,(%eax)
addl $0xff,(%eax)
addq $0xff,(%eax)
addb $0xff,0x7fffffff
addw $0xff,0x7fffffff
addl $0xff,0x7fffffff
addq $0xff,0x7fffffff
add %cl,(%eax)
add %cx,(%eax)
add %ecx,(%eax)
add %rcx,(%eax)
add (%eax),%cl
add (%eax),%cx
add (%eax),%ecx
add (%eax),%rcx
add %cl,0x7fffffff
add %cx,0x7fffffff
add %ecx,0x7fffffff
add %rcx,0x7fffffff
add 0x7fffffff,%cl
add 0x7fffffff,%cx
add 0x7fffffff,%ecx
add 0x7fffffff,%rcx
nop
sub $0xff,%al
sub $0xff,%cl
sub $0xff,%ax
sub $0xff,%cx
sub $0xff,%eax
sub $0xff,%ecx
sub $0xff,%rax
sub $0xff,%rcx
subb $0xff,(%eax)
subw $0xff,(%eax)
subl $0xff,(%eax)
subq $0xff,(%eax)
subb $0xff,0x7fffffff
subw $0xff,0x7fffffff
subl $0xff,0x7fffffff
subq $0xff,0x7fffffff
sub %cl,(%eax)
sub %cx,(%eax)
sub %ecx,(%eax)
sub %rcx,(%eax)
sub (%eax),%cl
sub (%eax),%cx
sub (%eax),%ecx
sub (%eax),%rcx
sub %cl,0x7fffffff
sub %cx,0x7fffffff
sub %ecx,0x7fffffff
sub %rcx,0x7fffffff
sub 0x7fffffff,%cl
sub 0x7fffffff,%cx
sub 0x7fffffff,%ecx
sub 0x7fffffff,%rcx
