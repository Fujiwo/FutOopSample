using System;

namespace じゃんけんゲーム.csharp
{
    class Utility
    {
        static Random random = new ();

        public static int GetRandom(int minimum, int maximum)
        {
            return random.Next(minimum, maximum);
        }
    };

    class じゃんけん
    {
        public enum 手
        {
            グー, チョキ, パー
        }

        public enum 結果
        {
            勝ち, 負け, あいこ
        }

        public static 結果 勝負(手 手1, 手 手2)
        {
            if (勝ちである(手1, 手2)) return 結果.勝ち  ;
            if (負けである(手1, 手2)) return 結果.負け  ;
                                      return 結果.あいこ;
        }

        static bool あいこである(手 手1, 手 手2) => 手1 == 手2;
        static bool 勝ちである  (手 手1, 手 手2) => IsPrevious(手1, 手2);
        static bool 負けである  (手 手1, 手 手2) => IsNext    (手1, 手2);
        static bool IsNext      (手 手1, 手 手2) => IsPrevious(手2, 手1);
        static bool IsPrevious  (手 手1, 手 手2) => ((int)手1 + 1) % 3 == (int)手2;
    }

    class じゃんけんUI
    {
        static string[] 勝負結果文字列 = { "勝ちました。", "負けました。", "あいこです。" };

        public static じゃんけん.手 手の取得()
        {
            for (; ; ) {
                var userInputCharacter = GetUserInputCharacter("手を入力してください (グー: g, チョキ: c, パー: p)");
                if (手に変換してみる(userInputCharacter, out var 手))
                    return 手;
            }
        }

        public static void 結果の表示(じゃんけん.結果 結果) => 表示(勝負結果文字列[(int)結果]);
        public static void 表示(string message) => Console.WriteLine(message);

        static bool 手に変換してみる(char character, out じゃんけん.手 手)
        {
            switch (char.ToLower(character))
            {
                case 'g': 手 = じゃんけん.手.グー  ; break;
                case 'c': 手 = じゃんけん.手.チョキ; break;
                case 'p': 手 = じゃんけん.手.パー  ; break;
                default : 手 = じゃんけん.手.グー  ; return false;
            }
            return true;
        }

        static char GetUserInputCharacter(string message)
        {
            Console.Write(message + " :");
            var key = Console.ReadKey();
            Console.WriteLine();
            return key.KeyChar;
        }
    }

    class User
    {
        public じゃんけん.手 手の取得() => じゃんけんUI.手の取得();
    }

    class Computer
    {
        public じゃんけん.手 手の取得() => (じゃんけん.手)Utility.GetRandom(0, 2);
    }

    class Set
    {
        User     user     = new User    ();
        Computer computer = new Computer();

        public じゃんけん.結果 Run()
        {
            var userの手     = user.手の取得();
            var computerの手 = computer.手の取得();
            じゃんけんUI.表示($"Computer の手は{computerの手}です。");
            var 結果         = じゃんけん.勝負(userの手, computerの手);
            じゃんけんUI.結果の表示(結果);
            return 結果;
        }
    }

    class Game
    {
        readonly int n回勝ち勝負;
        int          勝った回数 = 0;
        int          負けた回数 = 0;

        public Game(int n回勝ち勝負) => this.n回勝ち勝負 = n回勝ち勝負;

        public じゃんけん.結果 Run()
        {
            for (var set = new Set(); ;) {
                結果の設定(set.Run());
                途中経過の表示();
                if (勝った回数 >= n回勝ち勝負)
                    return じゃんけん.結果.勝ち;
                if (負けた回数 >= n回勝ち勝負)
                    return じゃんけん.結果.負け;
            }
        }

        void 結果の設定(じゃんけん.結果 結果)
        {
            switch (結果)
            {
                case じゃんけん.結果.勝ち: 勝った回数++; break;
                case じゃんけん.結果.負け: 負けた回数++; break;
                default                  :               break;
            }
        }

        void 途中経過の表示() => じゃんけんUI.表示(途中経過文字列の取得());
        string 途中経過文字列の取得() => $"勝ち: {勝った回数}回, 負け: {負けた回数}回";
    }

    class Program
    {
        static void Main() => じゃんけんUI.結果の表示(new Game(5).Run());
    }
}