using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Windows;

using WrapperCLI;

namespace ArithmeticWPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            //var ap = new WrapperCLI.ArithmeticParserWrapper();
            ArithmeticParser = new ArithmeticParserWrapper();
        }
        private ArithmeticParserWrapper ArithmeticParser;

        private Regex regex = new Regex(@"(?x)
                ^
                (?> (?<p> \( )* (?>-?\d+(?:\.\d+)?) (?<-p> \) )* )
                (?>(?:
                    [-+*/]
                    (?> (?<p> \( )* (?>-?\d+(?:\.\d+)?) (?<-p> \) )* )
                )*)
                (?(p)(?!))
                $
            ");
        private void ComputeButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (regex.IsMatch(ExpressionText.Text))
                {
                    if (ArithmeticParser != null)
                    {
                        var result = ArithmeticParser.ParseExpression(ExpressionText.Text);
                        ResultText.Text = result.ToString();
                    }
                }
                else
                {
                    MessageBox.Show("Wrong expression!");
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void ShowHistory_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var listCore = ArithmeticParser.GetHistory();
                var list = new List<HistoryUnit>();
                foreach (var item in listCore)
                {
                    var unit = new HistoryUnit() { Date = item.Date, Express = item.Expression, Result = item.Result };
                    list.Add(unit);
                }
                var historyDC = new HistoryDataContext(list);
                var hstr = new History();
                hstr.DataContext = historyDC;
                hstr.Show();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
    }
}
