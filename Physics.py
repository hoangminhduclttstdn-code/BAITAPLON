import sympy as sp
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# --- I. NHẬP THÔNG SỐ BAN ĐẦU ---

while True:
    try:
        h_input = input("Nhập Độ cao ban đầu H (mét, ví dụ: 20.0): ")
        H_initial = float(h_input)
        if H_initial <= 0:
            print("LỖI: Độ cao phải là một giá trị dương. Vui lòng nhập lại.")
            continue
        break # Thoát khỏi vòng lặp nếu nhập thành công và H dương
    except ValueError:
        print("LỖI: Giá trị nhập vào không hợp lệ. Vui lòng nhập số (ví dụ: 20.0).")

# Vòng lặp để bắt người dùng nhập lại cho đến khi Delta T hợp lệ
while True:
    try:
        deltaT_input = input("Nhập Chênh lệch thời gian chạm đất Delta T (giây, ví dụ: 2.0): ")
        DeltaT = float(deltaT_input)
        if DeltaT < 0:
            print("LỖI: Chênh lệch thời gian không được âm. Vui lòng nhập lại.")
            continue
        break # Thoát khỏi vòng lặp nếu nhập thành công và DeltaT không âm
    except ValueError:
        print("LỖI: Giá trị nhập vào không hợp lệ. Vui lòng nhập số (ví dụ: 2.0).")
    
G_ACCEL = 10.0     # Gia tốc trọng trường (giá trị cố định)

# --- II. GIẢI PHÁP SYMBOLIC (TÍCH PHÂN TỪ GỐC) ---

# Khai báo biến symbolic
sp.var('t H G t_A t_B v_0 C1 C2')

# 1. Tích phân từ Gia tốc (a = -G)
a = -G
v = sp.integrate(a, t) + C1 
y_general = sp.integrate(v, t) + C2 

# Vật A (Ném lên): y_A(t=0)=H và v_A(t=0)=v_0
C2_A_sol = sp.solve(sp.Eq(y_general.subs(t, 0), H), C2)[0] 
C1_A_sol = sp.solve(sp.Eq(v.subs(t, 0), v_0), C1)[0] 
y_A_sym = y_general.subs({C1: C1_A_sol, C2: C2_A_sol}) 

# Vật B (Rơi tự do)
C2_B_sol = H 
C1_B_sol = 0 
y_B_sym_check = y_general.subs({C1: C1_B_sol, C2: C2_B_sol}) 

# 2. Tính toán kết quả
t_B_sym = sp.solve(sp.Eq(y_B_sym_check.subs(t, t_B), 0), t_B)[1]
t_B_val = t_B_sym.subs({H: H_initial, G: G_ACCEL})
t_B_num = float(t_B_val)

t_A_num = t_B_num + DeltaT 
v_0_sym = sp.solve(sp.Eq(y_A_sym.subs(t, t_A), 0), v_0)[0]
v_0_num = float(v_0_sym.subs({H: H_initial, G: G_ACCEL, t_A: t_A_num}))

print(f"\n--- KẾT QUẢ TÍNH TOÁN ---")
print(f"Thời gian rơi B (t_B): {t_B_num:.2f} s")
print(f"Thời gian chạm đất A (t_A): {t_A_num:.2f} s")
print(f"Vận tốc ném lên (v_0): {v_0_num:.2f} m/s\n")


# --- III. TRỰC QUAN HÓA ĐỘNG (Đồ thị t-y) ---

# 1. Chuyển biểu thức symbolic sang hàm numpy
y_A_func = sp.lambdify((t, sp.Symbol('v_0'), H, G), y_A_sym, 'numpy')
y_B_func = sp.lambdify((t, H, G), y_B_sym_check, 'numpy') 

# 2. Tạo dữ liệu thời gian
t_final = t_A_num
time_steps = 200      
t_data = np.linspace(0, t_final, time_steps)

# 3. Thiết lập đồ thị (t-y)
fig, ax = plt.subplots(figsize=(10, 7))

y_peak = y_A_func(v_0_num / G_ACCEL, v_0_num, H_initial, G_ACCEL)

# TIÊU ĐỀ IN ĐẬM
title_text = f"Đồ Thị Biểu Diễn Độ Cao $y$ Theo Thời Gian $t$\n(Vận tốc ném lên $v_0$ = {v_0_num:.2f} m/s)"
ax.set_title(title_text, fontsize=14, fontweight='bold') 

ax.set_xlim(0, t_final)
ax.set_ylim(0, y_peak * 1.05) 
ax.set_xlabel("Thời gian t (giây)")
ax.set_ylabel("Độ cao y (mét)")
ax.grid(True, linestyle='--', alpha=0.6)

# KHỞI TẠO CÁC ĐƯỜNG VÀ ĐIỂM
# Dòng 96 ĐÃ SỬA LỖI: BỎ [0]. Gán trực tiếp đối tượng Line2D.
ground_line = ax.axhline(0, color='k', linestyle='-', linewidth=1.5, zorder=0) 

# Trail and Head for Object A (Red)
trail_A, = ax.plot([], [], linestyle='-', color='red', alpha=0.5)
head_A, = ax.plot([], [], 'o', color='red', markersize=8)

# Trail and Head for Object B (Blue)
trail_B, = ax.plot([], [], linestyle='--', color='blue', alpha=0.5)
head_B, = ax.plot([], [], '^', color='blue', markersize=8)

time_text = ax.text(0.05, 0.95, '', transform=ax.transAxes) 

# --- LEGEND VÀ ANNOTATION ---

# Tạo handles cho các điểm chạm đất (Lưu ý: ax.plot([],[]) trả về list, nên vẫn cần [0])
hit_A_handle = ax.plot([], [], 'o', color='red', markersize=8)[0]
hit_B_handle = ax.plot([], [], 's', color='blue', markersize=8)[0]

# Danh sách các đối tượng (handles) và nhãn (labels) cho Legend chi tiết
final_handles = [
    trail_A, 
    hit_A_handle,
    trail_B, 
    hit_B_handle,
    ground_line # ĐÃ SỬA: Không dùng [0] vì nó đã là Line2D
]
final_labels = [
    f'Vật A (Ném lên, $v_0$={v_0_num:.2f}m/s)',
    f'A chạm đất ({t_A_num:.2f}s)',
    f'Vật B (Rơi tự do, $v_0$=0m/s)',
    f'B chạm đất ({t_B_num:.2f}s)',
    'Mặt đất ($y=0m$)'
]

ax.legend(final_handles, final_labels, loc='upper right')

# 4. Hàm khởi tạo 
def init():
    # Xóa dữ liệu cũ
    trail_A.set_data([], []); head_A.set_data([], [])
    trail_B.set_data([], []); head_B.set_data([], [])
    time_text.set_text('Thời gian: 0.00 s')
    return trail_A, head_A, trail_B, head_B, time_text

# Lists to store path data (Trail)
t_A_list, y_A_list = [], []
t_B_list, y_B_list = [], []

# 5. Hàm cập nhật Animation
def update(frame):
    current_t = t_data[frame]

    # Tính vị trí Vật A
    y_A_current = y_A_func(current_t, v_0_num, H_initial, G_ACCEL)
    
    if y_A_current >= 0 and current_t <= t_A_num:
        t_A_list.append(current_t)
        y_A_list.append(y_A_current)
        
        trail_A.set_data(t_A_list, y_A_list)
        head_A.set_data([current_t], [y_A_current]) 
    else:
        # Dừng ở điểm chạm đất (t_A, 0)
        head_A.set_data([t_A_num], [0])
        if t_A_list and t_A_list[-1] < t_A_num:
            t_A_list.append(t_A_num)
            y_A_list.append(0)
            trail_A.set_data(t_A_list, y_A_list)

    # Tính vị trí Vật B
    y_B_current = y_B_func(current_t, H_initial, G_ACCEL)

    if y_B_current >= 0 and current_t <= t_B_num:
        t_B_list.append(current_t)
        y_B_list.append(y_B_current)
        
        trail_B.set_data(t_B_list, y_B_list)
        head_B.set_data([current_t], [y_B_current]) 
    else:
        # Dừng ở điểm chạm đất (t_B, 0)
        head_B.set_data([t_B_num], [0])
        if t_B_list and t_B_list[-1] < t_B_num:
            t_B_list.append(t_B_num)
            y_B_list.append(0)
            trail_B.set_data(t_B_list, y_B_list)
            
    # Hiển thị thời gian (tăng tính tương quan động)
    time_text.set_text(f'Thời gian: {current_t:.2f} s')

    return trail_A, head_A, trail_B, head_B, time_text

# 6. Chạy Animation
ani = FuncAnimation(fig, update, frames=len(t_data), interval=t_final*1000/len(t_data), 
                    init_func=init, blit=True, repeat=False)

plt.show()