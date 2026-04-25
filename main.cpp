#include <iostream>
#include <stdio.h>
//#include <cstdlib> // rand ve srand için
#include <ctime> // time için
#include <random> // modern random kütüphanesi
#include <fstream> //dosya işlemleri için

float Kp = 0.5;
float Ki = 0.5;
float Kd = 0.05;

float setpoint = 100.0;
float current_altitude = 0.0;
float dt = 0.1;
float integral = 0.0;
float previous_error = 0.0;

int main()
{

    // (Eski yöntem)Sensörlerdeki hata payını simüle edebilmek için random sayı üretme
    //srand(static_cast<unsigned int>(time(0)));
    
    //Modern yöntem
    /*
    // 1. Seed oluşturucu
    std::random_device rd;

    // 2. Motoru seed ile başlatma(Mersenne Twister)
    std::mt19937 generator(rd());
    */

    //üstteki yöntem hata verdiği için time ile seed yapıp motoru başlatma
    std::mt19937 generator(static_cast<unsigned int>(time(0)));

    // 3. Aralığı belirleme(noise için)
    std::uniform_real_distribution<double> noise_dist(-1.0,1.0);

    // Wind için 
    std::uniform_int_distribution<int> wind_dist(20,100);

    // rüzgar efektini rastgele anda vurma.
    int wind1 = wind_dist(generator);
    int wind2 = wind_dist(generator);

    // Veri kaydı için dosya açma
    std::ofstream file;
    file.open("datas.csv");

    // Dosya açma başarılı mı
    if(!file.is_open()){
        std::cerr << "Dosya acilamadi" << std::endl;
        return 1;
    }
    
    //Başlık satırları
    file << "adim; gercek_irtifa; olculen_irtifa; output; hedef" << "\n";
    
    for(int i=0; i<200 ; i++){

        // 4. rastgele gürültü üretme(Her adım için ayrı)
        float noise = noise_dist(generator); 

        //Rüzgar ile ani irtifa kaybı simülasyonu
        if (i == wind1 || i == wind2) {
            current_altitude -= 20;
            printf("*** RÜZGAR ÇARPTI***\n");
        }
    
        float measured_altitude = current_altitude + noise;

        //anlık hedefe uzaklık
        float error = setpoint - measured_altitude;

        // zamanla hata birikimi (sürekli dış etkenleri dengeler)
        integral = integral + (error * dt);

        // hatanını değişim hızı -> türev
        float derivative = (error - previous_error) / dt;

        // P + I + D -> motora gidecek güç
        float output = (Kp * error) + (Ki * integral) + (Kd * derivative);

        // yeni drone irtifası
        current_altitude = current_altitude + (output * dt);

       // sonraki adım için şimdiki hatayı saklama
       previous_error = error;

       printf("adim : %d  irtifa : %.2f  output : %.2f\n", i, current_altitude, output);

       // verileri dosyaya yazma CSV
       file << i << ";" << current_altitude << ";" << measured_altitude << ";" << output << ";" << setpoint << "\n"; 
    }

    file.close();
    printf("Veriler basariyla kaydedildi.\n");

    return 0;
}

