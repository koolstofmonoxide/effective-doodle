#include <../libs/TFminiPlus/TFminiPlus.h>
#include <vector>
#include <Wire.h>
#include <Arduino.h>

TFminiPlus lidar(0x10);

std::vector<float> speeds;

TaskHandle_t lidarTaskHandle = NULL;
TaskHandle_t processTaskHandle = NULL;

QueueHandle_t speedQueue;

void setup()
{
    Serial.begin(9600);

    // Start I2C communication
    lidar.begin();

    // Enable output
    lidar.enableOutput();

    Serial.print("Firmware version: ");
    Serial.println(lidar.getFirmwareVersion());

    // Set framerate to 200Hz
    lidar.setFrameRate(200);
    lidar.save();

    speedQueue = xQueueCreate(10, sizeof(float));

    xTaskCreatePinnedToCore(lidarTask, "Lidar Task", 2048, NULL, 1, &lidarTaskHandle, 0);
    xTaskCreatePinnedToCore(processTask, "Process Task", 2048, NULL, 1, &processTaskHandle, 1);
}

void lidarTask(void *pvParameters)
{
    static long int previousMicros = 0;
    static long int previousDistance = 0;

    while (true)
    {
        long int currentMicros = millis();
        long int timeDeltaMicros = currentMicros - previousMicros;

        long int currentDistance = lidar.getDistancemm();

        float speed = 0.0;
        if (timeDeltaMicros > 0)
        {
            speed = 1000.0f * (float(currentDistance) - float(previousDistance)) / float(timeDeltaMicros);
        }

        previousMicros = currentMicros;
        previousDistance = currentDistance;

        if (xQueueSend(speedQueue, &speed, portMAX_DELAY) != pdTRUE) // Put the speed in the queue
        {
            Serial.println("Failed to send speed to the queue.");
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

float medianFilter(std::vector<float> &data)
{
    const int n = 5;

    if (data.empty() || data.size() < n)
    {
        return 0.0;
    }

    std::vector<float> recentSpeeds(data.end() - n, data.end());

    std::sort(recentSpeeds.begin(), recentSpeeds.end());

    return recentSpeeds[n / 2]; // Return middle variable
}

float movingAverageFilter(std::vector<float> &data)
{
    if (data.empty() || data.size() < 5)
    {
        return 0.0;
    }

    float sum = 0.0;

    for (float value : data)
    {
        sum += value;
    }

    return sum / data.size();
}

void processTask(void *pvParameters)
{
    while (true)
    {
        float speed;
        if (xQueueReceive(speedQueue, &speed, portMAX_DELAY) == pdTRUE)
        {
            speed = abs(speed);
            speeds.push_back(speed);

            if (speeds.size() >= 5) // Start calculating once list contains 5 elements.
            {
                float medianSpeed = medianFilter(speeds);
                float movingAvgSpeed = movingAverageFilter(speeds);
                Serial.print(medianSpeed / 10);
                Serial.print(" cm/s, ");
                Serial.print(movingAvgSpeed / 10);
                Serial.println(" cm/s.");

                speeds.erase(speeds.begin()); // Keep buffer size at 5
            }
        }
    }
}

void loop()
{
}