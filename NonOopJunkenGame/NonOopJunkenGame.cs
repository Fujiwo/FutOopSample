/*
[C#演習] じゃんけんゲーム
※ オブジェクト指向でプログラミングを行うこと

- Player vs. Computer で勝負を行う。
- 先に n回 (n は 1以上、Player が指定) 勝った方が勝ちでゲームエンド。

ex.
******** じゃんけんゲーム ********
何勝で優勝かを入力してください: 2

2回先に勝った方が優勝です。
1回目: 手を入力してください(1: グー、2: チョキ、3: パー): グー

私の手はパーです。
あなたの負けです。
0勝1敗

2回目: 手を入力してください(1: グー、2: チョキ、3: パー): グー
私の手はグーです。
あいこです。
0勝1敗

2回目: 手を入力してください(1: グー、2: チョキ、3: パー): チョキ
私の手はパーです。
あなたの勝ちです。
1勝1敗

3回目: 手を入力してください(1: グー、2: チョキ、3: パー): パー
私の手はグーです。
あなたの勝ちです。
2勝1敗

あなたが優勝です。
-----
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ConsoleApp2
{
    class Program
    {
        static void Main(string[] args)
        {
            int Count = 0;
            int Times = 0;
            int Win = 0;
            int player = 0;
            int cpu = 0;

            Random rnd = new Random();
            ConsoleView view = new ConsoleView();

            Times = view.ShowInitiaraize();

            while (Count <= Times)
            {

                view.ShowRequest();
                player = int.Parse(Console.ReadLine());
                if (player < 1 || player > 3)
                {
                    view.ShowTryRequest();
                    continue;
                }
                cpu = rnd.Next(1, 4);

                Count = view.ShowResult(cpu, player);

            }
            view.ShowEnd();
        }
    }


    // コンソールの入出力
    class ConsoleView
    {
        int Count = 1;
        int Win = 0;
        int Times = 0;

        public int ShowInitiaraize()
        {
            Console.WriteLine("******** じゃんけんゲーム********");

            Console.WriteLine("何勝で優勝かを入力してください:");
            int Times = int.Parse(Console.ReadLine());
            Console.WriteLine(Times + "回先に勝った方が優勝です。\n");
            return Times;
        }

        public void ShowEnd()
        {
            if (Win > Times)
                Console.WriteLine("\nあなたが優勝です。\n");
            else
                Console.WriteLine("\nわたしが優勝です。\n");
        }

        // 再入力を促す
        public void ShowTryRequest()
        {
            Console.WriteLine("\nもう一度入力してください\n");
        }

        public void ShowRequest()
        {
            Console.WriteLine(Count + "回目: 手入力してください(1: グー、2: チョキ、3: パー): ");
        }

        public void ShowTie()
        {
            Console.WriteLine("あいこです。\n");
            Console.WriteLine("\nもう一度入力してください\n");
            Console.WriteLine(Count + "回目: 手入力してください(1: グー、2: チョキ、3: パー): ");
        }

        public int ShowResult(int cpu, int player)
        {
            switch (cpu)
            {
                case 1:
                    Console.WriteLine("私の手はグーです。\n");
                    break;
                case 2:
                    Console.WriteLine("私の手はチョキです。\n");
                    break;
                case 3:
                    Console.WriteLine("私の手はパーです。\n");
                    break;
            }

            if (cpu == player)
            {
                Console.WriteLine("あいこです。\n");
            }
            else
            {
                Count++;
                Times++;
            }
            if (player == 1 && cpu == 2)
            {
                Console.WriteLine("\nあなたの勝ちです\n");
                Win++;
            }
            else if (player == 2 && cpu == 3)
            {
                Console.WriteLine("\nあなたの勝ちです\n");
                Win++;
            }
            else if (player == 3 && cpu == 1)
            {
                Console.WriteLine("\nあなたの勝ちです\n");
                Win++;
            }
            return Times;
        }
    }

}
