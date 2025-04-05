import datetime

def find_day_of_week(year, month, day):
    try:
        date = datetime.date(year, month, day)
        days = ["월요일", "화요일", "수요일", "목요일", "금요일", "토요일", "일요일"]
        return days[date.weekday()]
    except ValueError:
        return "유효하지 않은 날짜입니다."

def main():
    year = int(input("몇 년도에 태어났나요? (예: 2000): "))
    month = int(input("몇 월에 태어났나요? (1-12): "))
    day = int(input("며칠이에요? (예: 15): "))
    
    day_of_week = find_day_of_week(year, month, day)
    print(f"{year}년 {month}월 {day}일은 {day_of_week}입니다.")

if __name__ == "__main__":
    main()
