#include "WPILib.h"


class Robot : public IterativeRobot
{
    Talon left;
    Talon right;
    Servo servo;
    Talon ytunnel;
    Talon armone;
    Talon armtwo;
    Talon Flywheelone;
    Talon Flywheeltwo;
    Joystick joy1;
    Joystick joy2;
    USBCamera cam0;
    AnalogGyro gyro1;
    double distance;
    double ang1;
    double ang2;
    int IsRotatingX;
    int IsRotatingY;
    int auto_number;
public:
    std::unique_ptr<Command> autonomousCommand;
    SendableChooser *chooser;
    std::shared_ptr<NetworkTable> table;

    Robot() :
            servo(0),
            left(1),
            right(2),
            ytunnel(5),
            Flywheelone(4),
            Flywheeltwo(3),
            armone(7),
            armtwo(8),
            joy1(1),
            joy2(0),
            cam0("cam2", true),
            gyro1(0),
            distance(0),
            ang1(0),
            ang2(0),
            auto_number(0)
    {
        cam0.SetBrightness(-10);
        cam0.SetExposureAuto();
        cam0.SetWhiteBalanceAuto();
        CameraServer::GetInstance()->SetQuality(50);
        CameraServer::GetInstance()->StartAutomaticCapture("cam2");
        table = NetworkTable::GetTable("SmartDashboard");
    }


private:
    void RobotInit()
    {
        /*CommandBase::init();
        chooser = new SendableChooser();
        chooser->AddDefault("Default Auto", new ExampleCommand());
        //chooser->AddObject("My Auto", new MyAutoCommand());
        SmartDashboard::PutData("Auto Modes", chooser);*/
        IsRotatingX = 0;
        gyro1.SetSensitivity(0.007);
    }


    void DisabledInit()
    {
    }

    void DisabledPeriodic()
    {
        Scheduler::GetInstance()->Run();
    }

    void AutonomousInit()
    {
        /* std::string autoSelected = SmartDashboard::GetString("Auto Selector", "Default");
        if(autoSelected == "My Auto") {
            autonomousCommand.reset(new MyAutoCommand());
        } else {
            autonomousCommand.reset(new ExampleCommand());
        } */

        autonomousCommand.reset((Command *) chooser->GetSelected());
        auto_number = 0;
        if (autonomousCommand != NULL)
            autonomousCommand->Start();
    }

    void AutonomousPeriodic()
    {
        left.Set(0.5);
        right.Set(0.5);
        Wait(2);
        left.StopMotor();
        right.StopMotor();
        gyro1.Reset();
        while (gyro1.GetAngle() <= 180){
            left.Set(0.3);
            right.Set(-0.3);
        }
        while (abs(ang2) >= 0.3)
        {
            distance = table->GetNumber("Distance", 0);
            ang1 = table->GetNumber("Angle1", 0);
            ang2 = table->GetNumber("Angle2", 0);
            AutoYTunnel();
        }
        while (abs(ang1) >= 0.3)
        {
            distance = table->GetNumber("Distance", 0);
            ang1 = table->GetNumber("Angle1", 0);
            ang2 = table->GetNumber("Angle2", 0);
            AutoXTunnel();
        }
        distance = table->GetNumber("Distance", 0);
        ang1 = table->GetNumber("Angle1", 0);
        ang2 = table->GetNumber("Angle2", 0);
        LaunchBall();
    }

    void TeleopInit()
    {
        if (autonomousCommand != NULL)
            autonomousCommand->Cancel();
        servo.SetAngle(0.0);
        left.StopMotor();
        right.StopMotor();
        ytunnel.StopMotor();
        Flywheelone.StopMotor();
        Flywheeltwo.StopMotor();
    }

    void RotateXTunnel(float speed, int IsAutoProgram)
    {
        // Alternate Plan
        left.Set(speed);
        right.Set(-speed);
        if (IsAutoProgram == 0) IsRotatingX = 1;
    }

    void StopXTunnel()
    {
        if (IsRotatingX == 1)
        {
            left.StopMotor();
            right.StopMotor();
            IsRotatingX = 0;
        }
    }

    void AutoXTunnel()
    {
        if (ang1 <= -8)
        {
            RotateXTunnel(0.043 * ang1 - 0.057, 1);
        }
        else if (ang1 <= -1)
        {
            RotateXTunnel(-0.1, 1);
        }
        else if (ang1 >= 8)
        {
            RotateXTunnel(0.043 * ang1 + 0.057, 1);
        }
        else if (ang1 >= 1)
        {
            RotateXTunnel(0.1, 1);
        }
        else
        {
            StopXTunnel();
        }
    }

    void AutoYTunnel()
    {
        if (ang2 <= -0.3)
        {
            RotateYTunnel(1, 1);
        }
        else if (ang2 >= 0.3)
        {
            RotateYTunnel(1, 1);
        }
        else
        {
            StopYTunnel();
        }
    }

    void StopYTunnel()
    {
        if (IsRotatingY == 1)
        {
            ytunnel.StopMotor();
            IsRotatingY = 0;
        }
    }

    void RotateYTunnel(float speed, int IsAutoProgram)
    {
        if (IsAutoProgram == 0) IsRotatingY = 1;
        ytunnel.Set(speed);
    }

    void TeleopPeriodic()
    {
        // joy for left joy1 for right joy1 for tunnel
        Scheduler::GetInstance()->Run();
        distance = table->GetNumber("Distance", 0);
        ang1 = table->GetNumber("Angle1", 0);
        ang2 = table->GetNumber("Angle2", 0);
        double forward_right_value = joy1.GetRawAxis(1);
        double forward_left_value = joy1.GetRawAxis(5);   //Standard Driver Station
        // double forward_left_value = joy1.GetRawAxis(3);   //Q Driver Station
        double twist_value = joy2.GetZ();
        double up_value = joy2.GetY();
        if (joy2.GetRawButton(1))
        {
            servo.SetAngle(30.0);
            servo.Set(1);
            Wait(1);
            servo.SetAngle(0.0);
            servo.Set(-1);
            servo.Set(0);
        }

        if (joy2.GetRawButton(2))
        {
            // !!!Warning!!! NO ABS!
            if (up_value > 0.1 || up_value < -0.1)
            {
                Flywheeltwo.Set(up_value);
                Flywheelone.Set(-up_value);
            }
            else
            {
                Flywheeltwo.Set(0);
                Flywheelone.Set(0);
            }
        }
        else
        {
            // !!!Warning!!! NO ABS!
            if (up_value > 0.1 || up_value < -0.1)
            {
                ytunnel.Set(up_value);
            }
            else
                ytunnel.Set(0);
        }


        // !!!Warning!!! NO ABS!
        if (forward_left_value > 0.1 || forward_left_value < -0.1)
            left.Set(forward_left_value * 0.8);
        else
            left.Set(0);

        // !!!Warning!!! NO ABS!
        if (forward_right_value > 0.1 || forward_right_value < -0.1)
            right.Set(-forward_right_value * 0.8);
        else
            right.Set(0);


        // !!!Warning!!! NO ABS!
        if (twist_value > 0.1 || twist_value < -0.1)
            // xtunnel.Set(twist_value / 5);

        if (joy2.GetRawButton(5))
        {
            armone.Set(0.8);
            armtwo.Set(0.8);
            Wait(2);
            armone.Set(0);
            armtwo.Set(0);
        }

        if (joy2.GetRawButton(6))
        {
            armone.Set(-0.8);
            armtwo.Set(-0.8);
            Wait(2);
            armone.Set(0);
            armtwo.Set(0);
        }

        if (joy2.GetRawButton(3))
        {
            if (fabs(ang1-3.88) >= 3)
                AutoXTunnel();
            else
                AutoYTunnel();
        }
        else
        {
            StopXTunnel();
            StopYTunnel();
        }


        if (joy2.GetRawButton(3))
        {
            AutoXTunnel();
            /* old codes
            if (ang1 <= 3.5){
                RotateXTunnel(-0.5);
            } else if (ang1 <= 5) {
                RotateXTunnel(-0.4);
            } else if (ang1 <= 6) {
                xtunnel.StopMotor();
            } else if (ang1 <= 9) {
                RotateXTunnel(0.4);
            } else {
                RotateXTunnel(0.5);
            }
            if (ang1 <= 6 && ang1 >= 5)
                xtunnel.Set(0);
            else {
                if (ang1 >= 6) {
                    if (ang1 >= 9) {
                        xtunnel.Set(-0.5);
                        Wait(0.1);
                        xtunnel.Set(0);
                        Wait(0.3);
                    }
                    else {
                        xtunnel.Set(-0.4);
                        Wait(0.1);
                        xtunnel.Set(0);
                        Wait(0.3);
                    }
                }
                else if (ang1 <= 5) {
                    if (ang1 <= 3.5) {
                        xtunnel.Set(0.5);
                        Wait(0.1);
                        xtunnel.Set(0);
                        Wait(0.3);
                    }
                    else {
                        xtunnel.Set(0.4);
                        Wait(0.1);
                        xtunnel.Set(0);
                        Wait(0.3);
                    }
                }
                else {
                    xtunnel.Set(0);
                }
            }
            old codes */

            AutoYTunnel();

            /* Fast Motors
            if (ang2 <= -4){
                RotateYTunnel(-0.3);
            } else if (ang2 >= 4){
                RotateYTunnel(0.3);
            } else if (abs(ang2) >= 0.3){
                RotateYTunnel(ang2 * 0.075);
            } else {
                ytunnel.StopMotor();
            }
            Fast Motors */

            /* old codes

            if (ang2 <= -8.5 && ang2 >= -10) {
                ytunnel.Set(0);
            }
            else {
                if (ang2 <= -10) {
                    if (ang2 <= -11) {
                        ytunnel.Set(-0.3);
                        Wait(0.1);
                        ytunnel.Set(0);
                        Wait(0.3);
                    }
                    else {
                        ytunnel.Set(-0.1);
                        Wait(0.2);
                        ytunnel.Set(0);
                        Wait(0.3);
                    }
                }
                else if (ang2 >= -8.5) {
                    if (ang2 >= -7) {
                        ytunnel.Set(0.3);
                        Wait(0.1);
                        ytunnel.Set(0);
                        Wait(0.3);
                    }
                    else {
                        ytunnel.Set(0.1);
                        Wait(0.2);
                        ytunnel.Set(0);
                        Wait(0.3);
                    }
                }
                else {
                    ytunnel.Set(0);
                }
            }
            old codes */
        }
        else
        {
            StopXTunnel();
            StopYTunnel();
        }

        if (joy2.GetRawButton(4)) LaunchBall();
    }

    void LaunchBall()
    {
        if (distance < 80)
        {
            Flywheelone.Set(-0.62);
            Flywheeltwo.Set(0.62);
        }
        else if (distance < 90)
        {
            Flywheelone.Set(-0.65);
            Flywheeltwo.Set(0.65);
        }
        else if (distance < 100)
        {
            Flywheelone.Set(-0.68);
            Flywheeltwo.Set(0.68);
        }
        else if (distance < 110)
        {
            Flywheelone.Set(-0.715);
            Flywheeltwo.Set(0.715);
        }
        else if (distance < 120)
        {
            Flywheelone.Set(-0.73);
            Flywheeltwo.Set(0.73);
        }
        else if (distance < 130)
        {
            Flywheelone.Set(-0.75);
            Flywheeltwo.Set(0.75);
        }
        else if (distance < 140)
        {
            Flywheelone.Set(-0.77);
            Flywheeltwo.Set(0.77);
        }
        else if (distance < 150)
        {
            Flywheelone.Set(-0.777);
            Flywheeltwo.Set(0.777);
        }
        Wait(3);
        servo.SetAngle(30.0);
        servo.Set(1);
        Wait(1);
        servo.SetAngle(0.0);
        servo.Set(-1);
        Flywheelone.Set(0);
        Flywheeltwo.Set(0);
    }

    void TestPeriodic()
    {
        LiveWindow::GetInstance()->Run();
    }
};

START_ROBOT_CLASS(Robot)

