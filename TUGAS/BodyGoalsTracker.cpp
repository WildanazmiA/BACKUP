// ================== BodyGoalsTracker_fixed.cpp ==================
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <climits>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <random>
#include <cstdio>

using namespace std;

// ================== Constants & Enums ==================
enum class GoalType { MAINTAIN, LOSE, GAIN };
enum class ActivityLevel { SEDENTARY = 1, LIGHT, MODERATE, ACTIVE, VERY_ACTIVE };
enum class MealTime { BREAKFAST, LUNCH, DINNER, SNACK };

const map<int, string> ACTIVITY_LEVELS = {
    {1, "Mager banget (jarang olahraga)"},
    {2, "Santai (olahraga 1-3x/minggu)"},
    {3, "Lumayan aktif (olahraga 3-5x/minggu)"},
    {4, "Aktif (olahraga 6-7x/minggu)"},
    {5, "Sangat aktif (olahraga setiap hari)"}
};

const map<MealTime, string> MEAL_TIMES = {
    {MealTime::BREAKFAST, "Sarapan 🍳"},
    {MealTime::LUNCH, "Makan Siang 🍲"},
    {MealTime::DINNER, "Makan Malam 🍛"},
    {MealTime::SNACK, "Cemilan 🍎"}
};

const vector<string> MOTIVATIONAL_QUOTES = {
    "💪 Gaskeun! Hari ini adalah kesempatan baru!",
    "🎯 Progress kecil tetap progress! Jangan nyerah!",
    "🔥 Kamu lebih kuat dari yang kamu kira!",
    "🌟 Konsisten adalah kunci keberhasilan!",
    "🚀 Semangat! Targetmu semakin dekat!",
    "💖 Sayangi tubuhmu dengan nutrisi yang tepat!",
    "🌈 Health is wealth! Investasi terbaik untuk masa depan!",
    "⭐ Setiap langkah kecil membawa ke perubahan besar!"
};

// ================== Forward declarations ==================
class InputHelper; // defined later

// ================== Structs ==================
struct UserProfile {
    string name;
    int age = 0;
    char gender = 'M';
    double height_cm = 0.0;
    double weight_kg = 0.0;
    ActivityLevel activity_level = ActivityLevel::SEDENTARY;
    GoalType goal = GoalType::MAINTAIN;
    double target_weight = 0.0;
    string last_active_date;

    double activity_factor() const {
        switch(activity_level) {
            case ActivityLevel::SEDENTARY: return 1.2;
            case ActivityLevel::LIGHT: return 1.375;
            case ActivityLevel::MODERATE: return 1.55;
            case ActivityLevel::ACTIVE: return 1.725;
            case ActivityLevel::VERY_ACTIVE: return 1.9;
            default: return 1.2;
        }
    }

    string goal_string() const {
        switch(goal) {
            case GoalType::MAINTAIN: return "Jaga Berat Badan";
            case GoalType::LOSE: return "Turunin Berat Badan";
            case GoalType::GAIN: return "Naikin Berat Badan";
            default: return "Unknown";
        }
    }
};

struct NutritionEntry {
    string date;
    MealTime meal_time;
    string food_name;
    int calories_in = 0;
    int protein_g = 0;
    int carbs_g = 0;
    int fat_g = 0;
};

struct ExerciseEntry {
    string date;
    int calories_out = 0;
    string exercise_type;
    int duration_minutes = 0;
    int intensity = 0; // 1-5 scale
};

struct DailySummary {
    string date;
    int total_calories_in = 0;
    int total_calories_out = 0;
    int total_protein = 0;
    int total_carbs = 0;
    int total_fat = 0;
    vector<string> exercises;
    bool completed_tracking = false;
};

struct DailyRecommendation {
    int target_calories = 0;
    int target_protein = 0;
    int target_carbs = 0;
    int target_fat = 0;
    int target_exercise_calories = 0;
    string motivational_message;
    vector<string> specific_advice;
};

// ================== InputHelper Class (moved up) ==================
class InputHelper {
public:
    static string get_string(const string &prompt, bool allow_empty = false) {
        string input;
        while (true) {
            cout << prompt;
            cout.flush();
            if(!std::getline(cin, input)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Error nih. Coba lagi ya.\n";
                continue;
            }

            if (!allow_empty && input.empty()) {
                cout << "Waduh, jangan kosong dong. Isi yang bener ya.\n";
                continue;
            }
            return input;
        }
    }

    static int get_int(const string &prompt, int min_val = INT_MIN, int max_val = INT_MAX) {
        while (true) {
            string input = get_string(prompt);
            try {
                size_t idx;
                int value = stoi(input, &idx);
                if (idx != input.size()) throw invalid_argument("extra chars");
                if (value < min_val || value > max_val) {
                    cout << "Masukin angka antara " << min_val << " sampe " << max_val << " dong.\n";
                    continue;
                }
                return value;
            } catch (const exception& e) {
                cout << "Hmm, kayanya bukan angka nih. Coba lagi ya.\n";
            }
        }
    }

    static double get_double(const string &prompt, double min_val = -numeric_limits<double>::max(), 
                           double max_val = numeric_limits<double>::max()) {
        while (true) {
            string input = get_string(prompt);
            try {
                size_t idx;
                double value = stod(input, &idx);
                if (idx != input.size()) throw invalid_argument("extra chars");
                if (value < min_val || value > max_val) {
                    cout << "Masukin angka antara " << min_val << " sampe " << max_val << " dong.\n";
                    continue;
                }
                return value;
            } catch (const exception& e) {
                cout << "Hmm, kayanya bukan angka nih. Coba lagi ya.\n";
            }
        }
    }

    static bool get_confirmation(const string &prompt) {
        while (true) {
            string input = get_string(prompt + " (y/n): ", false);
            if (input.empty()) continue;
            char choice = tolower(input[0]);
            if (choice == 'y' || choice == 's') return true; // 's' for "siap"/"ya" in some locales
            if (choice == 'n') return false;
            cout << "Masukin 'y' buat iya atau 'n' buat nggak dong.\n";
        }
    }

    static char get_gender() {
        while (true) {
            string input = get_string("Gender (L/P): ");
            if (input.empty()) continue;
            char gender = toupper(input[0]);
            if (gender == 'L' || gender == 'P') return (gender == 'L' ? 'M' : 'F');
            cout << "Masukin 'L' buat Laki-laki atau 'P' buat Perempuan dong.\n";
        }
    }

    static ActivityLevel get_activity_level() {
        cout << "\nLevel Aktivitas:\n";
        for (const auto &level : ACTIVITY_LEVELS) {
            cout << level.first << ". " << level.second << "\n";
        }
        
        while (true) {
            int choice = get_int("Pilih level aktivitas (1-5): ", 1, 5);
            return static_cast<ActivityLevel>(choice);
        }
    }

    static GoalType get_goal_type() {
        cout << "\nTarget Kamu:\n";
        cout << "1. Jaga berat badan sekarang\n";
        cout << "2. Turunin berat badan\n";
        cout << "3. Naikin berat badan (muscle gain)\n";
        
        while (true) {
            int choice = get_int("Pilih target (1-3): ", 1, 3);
            switch (choice) {
                case 1: return GoalType::MAINTAIN;
                case 2: return GoalType::LOSE;
                case 3: return GoalType::GAIN;
                default: return GoalType::MAINTAIN;
            }
        }
    }
};

// ================== Class ==================
class BodyGoalsTracker {
private:
    UserProfile profile;
    vector<NutritionEntry> nutrition_entries;
    vector<ExerciseEntry> exercise_entries;
    const string data_filename = "body_goals_data.csv";

public:
    // --- Profile Management ---
    void set_profile(const UserProfile &p) { 
        profile = p; 
        if (profile.target_weight <= 0) {
            profile.target_weight = profile.weight_kg;
        }
        profile.last_active_date = get_current_date();
    }
    
    UserProfile& get_profile() { return profile; }
    const UserProfile& get_profile() const { return profile; }

    // --- Entry Management ---
    void add_nutrition_entry(const NutritionEntry &e) { 
        nutrition_entries.push_back(e); 
    }
    
    void add_exercise_entry(const ExerciseEntry &e) { 
        exercise_entries.push_back(e); 
    }

    // --- Health Calculations ---
    double calculate_bmi(double weight = -1) const {
        if (weight <= 0) weight = profile.weight_kg;
        if(profile.height_cm <= 0) return 0.0;
        double h_m = profile.height_cm / 100.0;
        return weight / (h_m * h_m);
    }

    string get_bmi_category(double bmi) const {
        if(bmi <= 0) return "Unknown";
        if(bmi < 18.5) return "Kurang berat badan";
        if(bmi < 23.0) return "Normal (yasudah)";
        if(bmi < 27.5) return "Kelebihan berat badan";
        return "Obesitas (waduh)";
    }

    double calculate_bmr() const {
        if(profile.gender == 'M' || profile.gender == 'm')
            return 10.0 * profile.weight_kg + 6.25 * profile.height_cm - 5.0 * profile.age + 5.0;
        else
            return 10.0 * profile.weight_kg + 6.25 * profile.height_cm - 5.0 * profile.age - 161.0;
    }

    double calculate_tdee() const { 
        return calculate_bmr() * profile.activity_factor(); 
    }

    double calculate_recommended_calories() const {
        double tdee = calculate_tdee();
        switch(profile.goal) {
            case GoalType::LOSE: return max(1200.0, tdee - 500.0);
            case GoalType::GAIN: return tdee + 300.0;
            case GoalType::MAINTAIN: 
            default: return tdee;
        }
    }

    int calculate_days_to_goal() const {
        double weight_diff = profile.target_weight - profile.weight_kg;
        if (abs(weight_diff) < 0.1) return 0;
        
        double weekly_change = (profile.goal == GoalType::LOSE) ? -0.5 : 0.25;
        double days_needed = (weight_diff / weekly_change) * 7;
        return max(1, (int)ceil(abs(days_needed)));
    }

    // --- Smart Recommendations ---
    DailyRecommendation generate_daily_recommendations() const {
        DailyRecommendation rec;
        rec.target_calories = (int)round(calculate_recommended_calories());
        rec.target_protein = (int)round(profile.weight_kg * 2.2); // Increased for better results
        rec.target_carbs = (int)round((rec.target_calories * 0.45) / 4); // 45% from carbs
        rec.target_fat = (int)round((rec.target_calories * 0.25) / 9); // 25% from fat
        rec.target_exercise_calories = (profile.goal == GoalType::LOSE) ? 400 : 300;
        
        // Motivational message
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, (int)MOTIVATIONAL_QUOTES.size() - 1);
        rec.motivational_message = MOTIVATIONAL_QUOTES[dis(gen)];
        
        // Specific advice based on goal and progress
        auto today_summary = get_today_summary();
        int net_calories = today_summary.total_calories_in - today_summary.total_calories_out;
        
        if (profile.goal == GoalType::LOSE) {
            if (net_calories > rec.target_calories - 500) {
                rec.specific_advice.push_back("💡 Kurangi 200-300 kalori hari ini untuk tetap on track");
            }
            if (today_summary.total_protein < (int)round(rec.target_protein * 0.7)) {
                rec.specific_advice.push_back("🥚 Tambah protein untuk menjaga massa otot");
            }
        } else if (profile.goal == GoalType::GAIN) {
            if (net_calories < rec.target_calories) {
                rec.specific_advice.push_back("💡 Tambah kalori sehat untuk massa otot");
            }
            if (today_summary.total_protein < rec.target_protein) {
                rec.specific_advice.push_back("💪 Protein ekstra penting untuk muscle gain");
            }
        }
        
        // Exercise advice
        if (today_summary.total_calories_out < rec.target_exercise_calories * 0.5) {
            rec.specific_advice.push_back("🏃‍♂ Coba olahraga 30 menit hari ini");
        }
        
        return rec;
    }

    // --- Automatic Daily Check-in ---
    bool should_do_daily_checkin() const {
        string today = get_current_date();
        return profile.last_active_date != today;
    }

    void perform_automatic_daily_checkin() {
        if (!should_do_daily_checkin()) return;
        
        string today = get_current_date();
        cout << "\n🎉 SELAMAT PAGI, " << profile.name << "! 🎉\n";
        cout << "📅 Hari ini: " << today << "\n";
        cout << string(40, '=') << "\n";
        
        // Show motivational quote
        DailyRecommendation rec = generate_daily_recommendations();
        cout << "💫 " << rec.motivational_message << "\n\n";
        
        // Show today's targets
        cout << "🎯 TARGET HARI INI:\n";
        cout << "• Kalori: " << rec.target_calories << " kcal\n";
        cout << "• Protein: " << rec.target_protein << "g\n";
        cout << "• Karbohidrat: " << rec.target_carbs << "g\n";
        cout << "• Lemak: " << rec.target_fat << "g\n";
        cout << "• Olahraga: " << rec.target_exercise_calories << " kcal\n";
        
        // Check if user wants to do quick entry
        if (InputHelper::get_confirmation("\nMau catat sarapan sekalian?")) {
            add_meal_suggestion(MealTime::BREAKFAST);
        }
        
        profile.last_active_date = today;
    }

    void add_meal_suggestion(MealTime meal_time) {
        cout << "\n🍽  " << MEAL_TIMES.at(meal_time) << "\n";
        
        vector<string> suggestions;
        if (meal_time == MealTime::BREAKFAST) {
            suggestions = {
                "Oatmeal dengan buah 🍓 (300 kcal, 15g protein)",
                "Telur rebus + roti gandum 🥚 (350 kcal, 20g protein)", 
                "Smoothie bowl 🍌 (400 kcal, 18g protein)"
            };
        } else if (meal_time == MealTime::LUNCH) {
            suggestions = {
                "Nasi merah + ayam panggang + sayur 🍗 (500 kcal, 35g protein)",
                "Salad dengan tuna 🥗 (450 kcal, 30g protein)",
                "Quinoa bowl dengan sayuran 🥑 (480 kcal, 25g protein)"
            };
        } else if (meal_time == MealTime::DINNER) {
            suggestions = {
                "Ikan bakar + ubi + brokoli 🐟 (450 kcal, 40g protein)",
                "Tahu tempe + sayuran 🥦 (400 kcal, 25g protein)",
                "Sup ayam dengan sayuran 🍲 (380 kcal, 30g protein)"
            };
        } else {
            suggestions = {
                "Greek yogurt + kacang 🥜 (200 kcal, 15g protein)",
                "Apel + selai kacang 🍎 (250 kcal, 10g protein)",
                "Edamame rebus 🟢 (150 kcal, 12g protein)"
            };
        }
        
        cout << "💡 Suggestions untuk " << MEAL_TIMES.at(meal_time) << ":\n";
        for (int i = 0; i < (int)suggestions.size(); i++) {
            cout << i+1 << ". " << suggestions[i] << "\n";
        }
        
        if (InputHelper::get_confirmation("Mau catat makanan ini?")) {
            NutritionEntry entry;
            entry.date = get_current_date();
            entry.meal_time = meal_time;
            entry.food_name = InputHelper::get_string("Nama makanan: ");
            entry.calories_in = InputHelper::get_int("Kalori: ", 0, 20000);
            entry.protein_g = InputHelper::get_int("Protein (g): ", 0, 2000);
            entry.carbs_g = InputHelper::get_int("Karbohidrat (g): ", 0, 2000);
            entry.fat_g = InputHelper::get_int("Lemak (g): ", 0, 2000);
            
            add_nutrition_entry(entry);
            cout << "✅ " << MEAL_TIMES.at(meal_time) << " udah dicatat!\n";
        }
    }

    // --- Progress Tracking & Alerts ---
    void check_daily_progress() {
        string today = get_current_date();
        DailySummary today_summary = get_today_summary();
        DailyRecommendation rec = generate_daily_recommendations();
        
        cout << "\n📊 PROGRESS CHECK " << today << "\n";
        cout << string(40, '=') << "\n";
        
        // Calories progress
        int calorie_balance = today_summary.total_calories_in - today_summary.total_calories_out;
        int remaining_calories = rec.target_calories - today_summary.total_calories_in;
        
        cout << "🔥 Kalori: " << today_summary.total_calories_in << "/" << rec.target_calories;
        if (remaining_calories > 0) {
            cout << " (" << remaining_calories << " kcal tersisa)\n";
        } else {
            cout << " (" << abs(remaining_calories) << " kcal kelebihan)\n";
        }
        
        // Macronutrients progress
        auto safe_percent = [](int have, int target){ if(target<=0) return 0; return (int)round((double)have * 100.0 / target); };
        cout << "🥚 Protein: " << today_summary.total_protein << "/" << rec.target_protein << "g (" 
             << safe_percent(today_summary.total_protein, rec.target_protein) << "%)\n";
        cout << "🍚 Karbohidrat: " << today_summary.total_carbs << "/" << rec.target_carbs << "g (" 
             << safe_percent(today_summary.total_carbs, rec.target_carbs) << "%)\n";
        cout << "🥑 Lemak: " << today_summary.total_fat << "/" << rec.target_fat << "g (" 
             << safe_percent(today_summary.total_fat, rec.target_fat) << "%)\n";
        
        // Exercise progress
        cout << "💪 Olahraga: " << today_summary.total_calories_out << "/" << rec.target_exercise_calories 
             << " kcal dibakar\n";
        
        // Generate alerts
        generate_progress_alerts(today_summary, rec);
    }

    void generate_progress_alerts(const DailySummary& summary, const DailyRecommendation& rec) {
        vector<string> alerts;
        
        // Calorie alerts
        if (summary.total_calories_in > rec.target_calories * 1.2) {
            alerts.push_back("🚨 Kalori masuk sudah melebihi target! Kurangi makan malam nanti");
        } else if (summary.total_calories_in < rec.target_calories * 0.5) {
            alerts.push_back("⚠ Kalori masuk masih kurang! Tambah asupan sehat");
        }
        
        // Protein alerts
        if (summary.total_protein < (int)round(rec.target_protein * 0.6)) {
            alerts.push_back("💪 Protein kurang! Tambah sumber protein di makan berikutnya");
        }
        
        // Exercise alerts
        if (summary.total_calories_out < rec.target_exercise_calories * 0.3) {
            alerts.push_back("🏃‍♂ Aktivitas fisik masih kurang! Coba jalan-jalan 15 menit");
        }
        
        // Positive reinforcement
        if (summary.total_protein >= rec.target_protein && summary.total_calories_in <= rec.target_calories) {
            alerts.push_back("✅ Excellent! Pola makan hari ini sangat baik!");
        }
        
        if (!alerts.empty()) {
            cout << "\n🔔 PEMBERITAHUAN:\n";
            for (const auto& alert : alerts) {
                cout << "• " << alert << "\n";
            }
        }
    }

    // --- Data Aggregation ---
    map<string, DailySummary> get_daily_summaries() const {
        map<string, DailySummary> summaries;
        
        // Process nutrition entries
        for (const auto &entry : nutrition_entries) {
            summaries[entry.date].date = entry.date;
            summaries[entry.date].total_calories_in += entry.calories_in;
            summaries[entry.date].total_protein += entry.protein_g;
            summaries[entry.date].total_carbs += entry.carbs_g;
            summaries[entry.date].total_fat += entry.fat_g;
        }
        
        // Process exercise entries
        for (const auto &entry : exercise_entries) {
            summaries[entry.date].date = entry.date;
            summaries[entry.date].total_calories_out += entry.calories_out;
            summaries[entry.date].exercises.push_back(
                entry.exercise_type + " (" + to_string(entry.duration_minutes) + " menit)"
            );
        }
        
        return summaries;
    }

    DailySummary get_today_summary() const {
        string today = get_current_date();
        auto summaries = get_daily_summaries();
        auto it = summaries.find(today);
        if (it != summaries.end()) return it->second;
        return DailySummary{today,0,0,0,0,0,{ },false};
    }

    // --- Simple comprehensive report ---
    void print_comprehensive_report() const {
        auto summaries = get_daily_summaries();
        cout << "\n--- 📈 LAPORAN RINGKAS ---\n";
        if (summaries.empty()) {
            cout << "Belum ada data untuk dibuat laporan.\n";
            return;
        }
        cout << left << setw(12) << "Tanggal" << setw(10) << "KaloriIn" << setw(12) << "KaloriOut"
             << setw(10) << "Protein" << setw(10) << "Karb" << setw(10) << "Lemak" << "\n";
        cout << string(70, '-') << "\n";
        for (const auto &p : summaries) {
            const auto &d = p.second;
            cout << left << setw(12) << d.date << setw(10) << d.total_calories_in << setw(12) << d.total_calories_out
                 << setw(10) << d.total_protein << setw(10) << d.total_carbs << setw(10) << d.total_fat << "\n";
        }
    }

    // --- File Operations ---
    bool save_to_file() const {
        ofstream ofs(data_filename);
        if(!ofs) return false;

        // Save profile with last active date
        ofs << "PROFILE," << profile.name << ',' << profile.age << ',' << profile.gender << ','
            << profile.height_cm << ',' << profile.weight_kg << ',' 
            << static_cast<int>(profile.activity_level) << ',' 
            << static_cast<int>(profile.goal) << ',' << profile.target_weight << ','
            << profile.last_active_date << '\n';
        
        // Save nutrition entries with meal time and food name
        ofs << "NUTRITION,Date,MealTime,FoodName,Calories,Protein,Carbs,Fat\n";
        for(const auto &e : nutrition_entries) {
            string food = e.food_name;
            replace(food.begin(), food.end(), ',', ';');
            ofs << "N," << e.date << ',' << static_cast<int>(e.meal_time) << ','
                << food << ',' << e.calories_in << ',' << e.protein_g << ',' 
                << e.carbs_g << ',' << e.fat_g << '\n';
        }
        
        // Save exercise entries with intensity
        ofs << "EXERCISE,Date,Calories,Type,Duration,Intensity\n";
        for(const auto &e : exercise_entries) {
            string type = e.exercise_type;
            replace(type.begin(), type.end(), ',', ';');
            ofs << "E," << e.date << ',' << e.calories_out << ',' << type << ',' 
                << e.duration_minutes << ',' << e.intensity << '\n';
        }
        
        return ofs.good();
    }

    bool load_from_file() {
        ifstream ifs(data_filename);
        if(!ifs) return false;

        nutrition_entries.clear();
        exercise_entries.clear();
        string line;

        while(getline(ifs, line)) {
            if(line.empty()) continue;
            
            stringstream ss(line);
            string type;
            getline(ss, type, ',');
            
            if (type == "PROFILE") {
                vector<string> tokens;
                string token;
                while(getline(ss, token, ',')) tokens.push_back(token);
                
                if(tokens.size() >= 9) {
                    profile.name = tokens[0];
                    profile.age = stoi(tokens[1]);
                    profile.gender = tokens[2].empty() ? 'M' : tokens[2][0];
                    profile.height_cm = stod(tokens[3]);
                    profile.weight_kg = stod(tokens[4]);
                    profile.activity_level = static_cast<ActivityLevel>(stoi(tokens[5]));
                    profile.goal = static_cast<GoalType>(stoi(tokens[6]));
                    profile.target_weight = stod(tokens[7]);
                    profile.last_active_date = tokens.size() > 8 ? tokens[8] : get_current_date();
                }
            }
            else if (type == "N") {
                NutritionEntry e;
                string meal_time, cal, prot, carb, fat;
                
                getline(ss, e.date, ',');
                getline(ss, meal_time, ',');
                getline(ss, e.food_name, ',');
                getline(ss, cal, ',');
                getline(ss, prot, ',');
                getline(ss, carb, ',');
                getline(ss, fat, ',');
                
                e.meal_time = static_cast<MealTime>(stoi(meal_time));
                if(!cal.empty()) e.calories_in = stoi(cal);
                if(!prot.empty()) e.protein_g = stoi(prot);
                if(!carb.empty()) e.carbs_g = stoi(carb);
                if(!fat.empty()) e.fat_g = stoi(fat);
                
                nutrition_entries.push_back(e);
            }
            else if (type == "E") {
                ExerciseEntry e;
                string cal, type_str, dur, intensity;
                
                getline(ss, e.date, ',');
                getline(ss, cal, ',');
                getline(ss, e.exercise_type, ',');
                getline(ss, dur, ',');
                getline(ss, intensity, ',');
                
                replace(e.exercise_type.begin(), e.exercise_type.end(), ';', ',');
                if(!cal.empty()) e.calories_out = stoi(cal);
                if(!dur.empty()) e.duration_minutes = stoi(dur);
                if(!intensity.empty()) e.intensity = stoi(intensity);
                
                exercise_entries.push_back(e);
            }
        }
        return true;
    }

    // --- Utility Functions ---
    static string get_current_date() {
        time_t t = time(nullptr);
        tm *lt = localtime(&t);
        char buf[32];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d", lt->tm_year+1900, lt->tm_mon+1, lt->tm_mday);
        return string(buf);
    }

    static string get_current_time() {
        time_t t = time(nullptr);
        tm *lt = localtime(&t);
        char buf[32];
        snprintf(buf, sizeof(buf), "%02d:%02d", lt->tm_hour, lt->tm_min);
        return string(buf);
    }
};

// ================== Enhanced Menu System ==================
class MenuSystem {
private:
    BodyGoalsTracker tracker;

    void clear_screen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void show_main_menu() {
        clear_screen();
        
        // Automatic daily check-in
        tracker.perform_automatic_daily_checkin();
        
        // Show quick progress
        tracker.check_daily_progress();
        
        cout << "\n" << string(50, '=') << "\n";
        cout << "           🏋‍♂ BODY GOALS TRACKER 🏋‍♀\n";
        cout << "                 Menu Utama\n";
        cout << string(50, '=') << "\n";
        cout << "1. 📝 Profil & Target\n";
        cout << "2. 🍽  Catat Makanan (Auto-suggest)\n";
        cout << "3. 💪 Catat Olahraga\n";
        cout << "4. 📊 Progress Hari Ini\n";
        cout << "5. 🎯 Rekomendasi Smart\n";
        cout << "6. 📈 Laporan Mingguan\n";
        cout << "7. ⚡ Quick Entry (All-in-One)\n";
        cout << "8. 💾 Save/Load Data\n";
        cout << "0. ❌ Keluar\n";
        cout << string(50, '=') << "\n";
    }

    void handle_smart_food_entry() {
        cout << "\n--- 🍽  SMART FOOD TRACKER ---\n";
        
        vector<MealTime> meal_times = {MealTime::BREAKFAST, MealTime::LUNCH, MealTime::DINNER, MealTime::SNACK};
        
        for (auto meal_time : meal_times) {
            if (InputHelper::get_confirmation("Mau catat " + MEAL_TIMES.at(meal_time) + "?")) {
                tracker.add_meal_suggestion(meal_time);
            }
        }
        
        // Auto-check progress after food entry
        tracker.check_daily_progress();
    }

    void handle_quick_all_in_one() {
        cout << "\n--- ⚡ QUICK ENTRY - ALL IN ONE ---\n";
        
        // Quick weight update
        if (InputHelper::get_confirmation("Update berat badan hari ini?")) {
            double new_weight = InputHelper::get_double("Berat badan terkini (kg): ", 2.0, 700.0);
            tracker.get_profile().weight_kg = new_weight;
            cout << "✅ Berat badan updated! ";
            
            double bmi = tracker.calculate_bmi();
            cout << "BMI sekarang: " << fixed << setprecision(1) << bmi << "\n";
        }
        
        // Quick food entry for all meals
        handle_smart_food_entry();
        
        // Quick exercise entry
        if (InputHelper::get_confirmation("Mau catat olahraga hari ini?")) {
            ExerciseEntry exercise;
            exercise.date = BodyGoalsTracker::get_current_date();
            exercise.exercise_type = InputHelper::get_string("Jenis olahraga: ");
            exercise.duration_minutes = InputHelper::get_int("Durasi (menit): ", 1, 1440);
            exercise.intensity = InputHelper::get_int("Intensitas (1-5): ", 1, 5);
            exercise.calories_out = InputHelper::get_int("Kalori dibakar: ", 0, 5000);
            
            tracker.add_exercise_entry(exercise);
            cout << "✅ Olahraga tercatat! Good job! 💪\n";
        }
        
        // Final progress check
        tracker.check_daily_progress();
    }

    void handle_smart_recommendations() {
        auto rec = tracker.generate_daily_recommendations();
        auto profile = tracker.get_profile();
        auto today_summary = tracker.get_today_summary();
        
        cout << "\n--- 🧠 SMART RECOMMENDATIONS ---\n";
        cout << "💡 " << rec.motivational_message << "\n\n";
        
        cout << "🎯 BERDASARKAN PROFIL KAMU:\n";
        cout << "• Target: " << profile.goal_string() << "\n";
        cout << "• Berat: " << profile.weight_kg << "kg → " << profile.target_weight << "kg\n";
        cout << "• Estimasi: " << tracker.calculate_days_to_goal() << " hari lagi\n\n";
        
        cout << "📋 REKOMENDASI SPESIFIK:\n";
        if (rec.specific_advice.empty()) {
            cout << "⭐ Pertahankan pola yang sekarang! Kamu sudah on track! \n";
        } else {
            for (const auto& advice : rec.specific_advice) {
                cout << "• " << advice << "\n";
            }
        }
        
        // Meal timing advice
        string current_time = BodyGoalsTracker::get_current_time();
        int hour = 0;
        try { hour = stoi(current_time.substr(0, 2)); } catch(...) { hour = 0; }
        
        if (hour < 10 && today_summary.total_calories_in < 300) {
            cout << "\n🍳 SARAN SARAPAN: Prioritaskan protein untuk energy seharian\n";
        } else if (hour >= 12 && hour < 14 && today_summary.total_calories_in < 800) {
            cout << "\n🍲 SARAN MAKAN SIANG: Kombinasi karbo kompleks + protein\n";
        } else if (hour >= 18 && today_summary.total_calories_in < rec.target_calories * 0.7) {
            cout << "\n🍛 SARAN MAKAN MALAM: Ringan tapi bergizi, hindari makan larut\n";
        }
    }

public:
    void run() {
        #ifdef _WIN32
            system("chcp 65001 > nul");
        #endif
        
        cout << "🚀 Starting Body Goals Tracker - Enhanced Version...\n";
        
        // Try to load existing data
        ifstream test_file("body_goals_data.csv");
        if (test_file) {
            test_file.close();
            if (InputHelper::get_confirmation("Ada data sebelumnya. Mau load nggak?")) {
                if (tracker.load_from_file()) {
                    cout << "✅ Data loaded! Welcome back! 🎉\n";
                    // Immediate daily check-in for returning users
                    tracker.perform_automatic_daily_checkin();
                }
            }
        }

        cout << "Press Enter untuk mulai...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        while (true) {
            show_main_menu();
            int choice = InputHelper::get_int("Pilih menu (0-8): ", 0, 8);
            
            switch (choice) {
                case 0:
                    if (InputHelper::get_confirmation("Simpan data sebelum keluar?")) {
                        tracker.save_to_file();
                    }
                    cout << "💖 Terima kasih! Semangat terus buat health journey-nya! 🌟\n";
                    return;
                    
                case 1: {
                    UserProfile profile;
                    cout << "\n--- 📋 Profil & Target ---\n";
                    profile.name = InputHelper::get_string("Nama: ");
                    profile.age = InputHelper::get_int("Umur: ", 1, 120);
                    profile.gender = InputHelper::get_gender();
                    profile.height_cm = InputHelper::get_double("Tinggi (cm): ", 30.0, 300.0);
                    profile.weight_kg = InputHelper::get_double("Berat sekarang (kg): ", 2.0, 700.0);
                    profile.activity_level = InputHelper::get_activity_level();
                    profile.goal = InputHelper::get_goal_type();
                    
                    if (profile.goal != GoalType::MAINTAIN) {
                        string prompt = "Target berat (kg): ";
                        profile.target_weight = InputHelper::get_double(prompt, 2.0, 700.0);
                    }
                    
                    tracker.set_profile(profile);
                    cout << "✅ Profil updated! Sekarang kita bisa mulai tracking! 🎯\n";
                    break;
                }
                    
                case 2: handle_smart_food_entry(); break;
                case 3: {
                    ExerciseEntry exercise;
                    cout << "\n--- 💪 Catat Olahraga ---\n";
                    exercise.date = BodyGoalsTracker::get_current_date();
                    exercise.exercise_type = InputHelper::get_string("Jenis olahraga: ");
                    exercise.duration_minutes = InputHelper::get_int("Durasi (menit): ", 1, 1440);
                    exercise.intensity = InputHelper::get_int("Intensitas (1-5): ", 1, 5);
                    exercise.calories_out = InputHelper::get_int("Kalori dibakar: ", 0, 5000);
                    
                    tracker.add_exercise_entry(exercise);
                    cout << "✅ Olahraga tercatat! Mantap! 💪\n";
                    tracker.check_daily_progress();
                    break;
                }
                case 4: tracker.check_daily_progress(); break;
                case 5: handle_smart_recommendations(); break;
                case 6: tracker.print_comprehensive_report(); break;
                case 7: handle_quick_all_in_one(); break;
                case 8: {
                    cout << "\n--- 💾 Data Management ---\n";
                    if (InputHelper::get_confirmation("Simpan data sekarang?")) {
                        tracker.save_to_file();
                    }
                    if (InputHelper::get_confirmation("Load data dari file?")) {
                        tracker.load_from_file();
                    }
                    break;
                }
            }
            
            cout << "\nPress Enter untuk lanjut...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
};

// ================== Main Function ==================
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << fixed << setprecision(1);
    setvbuf(stdout, nullptr, _IONBF, 0);
    
    try {
        MenuSystem app;
        app.run();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}