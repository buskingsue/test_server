import torch
print(torch.cuda.is_available())  # GPU가 사용 가능한지 여부를 반환
print(torch.cuda.current_device())  # 현재 사용 중인 GPU의 ID를 출력
print("\n")