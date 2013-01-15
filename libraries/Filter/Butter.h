template <typename Coefficients>
class Butter2
{
public:
  float filter(float input)
  {
        float newhist = input + Coefficients::A1*hist[1] + Coefficients::A2*hist[0];
        float ret = (newhist + 2*hist[1] + hist[0])/Coefficients::GAIN;
        hist[0] = hist[1]; hist[1] = newhist;
        return ret;
  }
private:
    float hist[2];
};

struct butter100_025_coeffs
{
  static const float A1 = 1.9777864838;
  static const float A2 = -0.9780305085;
  static const float GAIN = 1.639178228e+04;
};
typedef Butter2<butter100_025_coeffs> butter100hz0_25; //100hz sample, 0.25hz fcut
typedef Butter2<butter100_025_coeffs> butter50hz0_125; //50hz sample, 0.125hz fcut
typedef Butter2<butter100_025_coeffs> butter10hz0_025; //10hz sample, 0.025hz fcut

struct butter100_05_coeffs
{
  static const float A1 = 1.9555782403;
  static const float A2 = -0.9565436765;
  static const float GAIN = 4.143204922e+03;
};
typedef Butter2<butter100_05_coeffs> butter100hz0_5; //100hz sample, 0.5hz fcut
typedef Butter2<butter100_05_coeffs> butter50hz0_25; //50hz sample, 0.25hz fcut
typedef Butter2<butter100_05_coeffs> butter10hz0_05; //10hz sample, 0.05hz fcut

struct butter100_1_coeffs
{
  static const float A1 = 1.9111970674;
  static const float A2 = -0.9149758348;
  static const float GAIN = 1.058546241e+03;
};
typedef Butter2<butter100_1_coeffs> butter100hz1_0; //100hz sample, 1hz fcut
typedef Butter2<butter100_1_coeffs> butter50hz0_5; //50hz sample, 0.5hz fcut
typedef Butter2<butter100_1_coeffs> butter10hz0_1; //10hz sample, 0.1hz fcut

struct butter100_1_5_coeffs
{
  static const float A1 = 1.8668922797;
  static const float A2 = -0.8752145483;
  static const float GAIN = 4.806381793e+02;
};
typedef Butter2<butter100_1_5_coeffs> butter100hz1_5; //100hz sample, 1.5hz fcut
typedef Butter2<butter100_1_5_coeffs> butter50hz0_75; //50hz sample, 0.75hz fcut
typedef Butter2<butter100_1_5_coeffs> butter10hz0_15; //10hz sample, 0.15hz fcut

struct butter100_2_coeffs
{
  static const float A1 = 1.8226949252;
  static const float A2 = -0.8371816513;
  static const float GAIN = 2.761148367e+02;
};
typedef Butter2<butter100_2_coeffs> butter100hz2_0; //100hz sample, 2hz fcut
typedef Butter2<butter100_2_coeffs> butter50hz1_0; //50hz sample, 1hz fcut
typedef Butter2<butter100_2_coeffs> butter10hz0_2; //10hz sample, 0.2hz fcut

struct butter100_3_coeffs
{
  static const float A1 = 1.7347257688;
  static const float A2 = -0.7660066009;
  static const float GAIN = 1.278738361e+02;
};
typedef Butter2<butter100_3_coeffs> butter100hz3_0; //100hz sample, 3hz fcut
typedef Butter2<butter100_3_coeffs> butter50hz1_5; //50hz sample, 1.5hz fcut
typedef Butter2<butter100_3_coeffs> butter10hz0_3; //10hz sample, 0.3hz fcut

struct butter100_4_coeffs
{
  static const float A1 = 1.6474599811;
  static const float A2 = -0.7008967812;
  static const float GAIN = 7.485478157e+01;
};
typedef Butter2<butter100_4_coeffs> butter100hz4_0; //100hz sample, 4hz fcut
typedef Butter2<butter100_4_coeffs> butter50hz2_0; //50hz sample, 2hz fcut
typedef Butter2<butter100_4_coeffs> butter10hz0_4; //10hz sample, .4hz fcut

struct butter100_8_coeffs
{
  static const float A1 = 1.3072850288;
  static const float A2 = -0.4918122372;
  static const float GAIN = 2.167702007e+01;
};
typedef Butter2<butter100_8_coeffs> butter100hz8_0; //100hz sample, 8hz fcut
typedef Butter2<butter100_8_coeffs> butter50hz4_0; //50hz sample, 4hz fcut
typedef Butter2<butter100_8_coeffs> butter10hz0_8; //10hz sample, .8hz fcut

struct butter50_8_coeffs
{
  static const float A1 = 0.6710290908;
  static const float A2 = -0.2523246263;
  static const float GAIN = 6.881181354e+00;
};
typedef Butter2<butter50_8_coeffs> butter50hz8_0; //50hz sample, 8hz fcut
typedef Butter2<butter50_8_coeffs> butter10hz1_6; //10hz sample, 1.6hz fcut