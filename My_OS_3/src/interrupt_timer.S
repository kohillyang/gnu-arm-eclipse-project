
.global taskSwitchAsm
.global getNextSp
.global SysTick_handler
.type getNextSp function


//���뽫  SysTick_handler�����ɺ��������򽫻᳢���л���arm״̬������һ��Ӳfault
.type SysTick_handler function

SysTick_handler:
	push {lr}               //��ת��C����ʱ��ı�r0��lr��ֵ��r0�Ѿ���Ӳ���Զ�ѹջ,����lr���ɣ����ж���ʹ�õ���msp


	cpsid i         		//�ر��ж�
	bl Led_handler          //��˸led����ʾ�����жϺ���

	mrs r0,psp
	mov r1,sp                //Ŀ��stmָ������һ��С�鷳����push���ˣ���Ҫ����sp

	mov sp,r0
	push {r4-r7}         //����ǰ����ļĴ���ѹ��psp��
	mov r0,sp
/*
    *���ݵ�ǰ��ջָ���жϵ�ǰ������id��
    *���浱ǰ�������spָ�룬����Ϊr0
    *������һ�������pspָ��,�����Ĳ������ݵ��ù��򱣴���r0��
 	*r0�������¸������psp
 	*lr��ֵ�Ѿ��ı�
 	*��r0��ֵΪ0��ʱ���ʾ�����δ���й�
 */
	bl getNextSp

 //�����ı�����ϣ�����ջָ�����

	mov sp,r0         //�ӽ���ջ�е����¸�����ļĴ���
	pop {r4-r7}
	mov r0,sp

	msr psp,r0              //�����̶�ջָ��д��psr
	cpsie i                 //���ж�

	mov sp,r1              //�ָ�sp
	pop {pc}           //����pc����ת���¸������ִ��



