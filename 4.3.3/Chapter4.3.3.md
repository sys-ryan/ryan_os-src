세그먼트 레지스터에 비디오 메모리 어드레스 설정
```
mov ax, 0xB800
mov ds, ax
```

화면 최상단에 M을 표시하는 소스 코드
```
mov byte [0x00], 'M'
mov byte [0x01], 0x4A
```

# 실행 결과
빌드 후 QEMU를 실행하면 빨간 배경에 밝은 녹색으로 출력된 'M'을 볼 수 있다.
<img src="./img/4.3.3 qemu exe" title="4.3.3 qeum exe"></img><br/>
