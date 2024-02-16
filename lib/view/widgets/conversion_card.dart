import 'package:currency_convertor/utils/utils.dart';
import 'package:currency_convertor/view/widgets/dropdown_row.dart';
import 'package:flutter/material.dart';
import 'package:url_launcher/url_launcher.dart';

class ConversionCard extends StatefulWidget {
  final dynamic rates;
  final Map currencies;
  const ConversionCard({
    Key? key,
    required this.rates,
    required this.currencies,
  }) : super(key: key);

  @override
  State<ConversionCard> createState() => _ConversionCardState();
}

class _ConversionCardState extends State<ConversionCard> {
  TextEditingController amountController = TextEditingController();
  final GlobalKey<FormFieldState> formFieldKey = GlobalKey();
  String dropdownValue1 = 'USD';
  String dropdownValue2 = 'INR';
  String conversion = '';
  bool isLoading = false;

  void startLoading() {
    setState(() {
      isLoading = true;
    });
  }

  void stopLoading() {
    setState(() {
      isLoading = false;
    });
  }

  String sanitizeInput(String input) {
    return input.replaceAll(RegExp(r'[,\s]'), '');
  }

  void convertAndDisplay() {
    String sanitizedAmount = sanitizeInput(amountController.text);
    conversion =
        '$sanitizedAmount $dropdownValue1 = ${Utils.convert(widget.rates, sanitizedAmount, dropdownValue1, dropdownValue2)} $dropdownValue2';
    stopLoading();
  }

  void swapCurrencies() {
    setState(() {
      String temp = dropdownValue1;
      dropdownValue1 = dropdownValue2;
      dropdownValue2 = temp;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.fromLTRB(
          25, 100, 25, 25), // Add space from the top and padding on all edges
      child: Column(
        children: [
          TextFormField(
            key: formFieldKey,
            controller: amountController,
            decoration: const InputDecoration(
              hintText: 'Enter Amount',
            ),
            keyboardType: TextInputType.number,
            validator: (value) {
              if (value!.isEmpty) {
                return 'Please enter an amount';
              }
              return null;
            },
          ),
          const SizedBox(
            height: 50,
          ),
          DropdownRow(
            label: 'From:',
            value: dropdownValue1,
            currencies: widget.currencies,
            onChanged: (String? newValue) {
              setState(() {
                dropdownValue1 = newValue!;
              });
            },
          ),
          IconButton(
            icon: const Icon(Icons.swap_vert),
            onPressed: () {
              if (amountController.text.isEmpty) {
                swapCurrencies();
              } else {
                swapCurrencies();
                convertAndDisplay();
              }
            },
          ),
          DropdownRow(
            label: 'To:',
            value: dropdownValue2,
            currencies: widget.currencies,
            onChanged: (String? newValue) {
              setState(() {
                dropdownValue2 = newValue!;
              });
            },
          ),
          const SizedBox(
            height: 20,
          ),
          Row(
            children: [
              Expanded(
                child: ElevatedButton(
                    onPressed: () {
                      if (formFieldKey.currentState!.validate()) {
                        startLoading();
                        conversion = '';
                        convertAndDisplay();
                      }
                    },
                    child: isLoading
                        ? const CircularProgressIndicator()
                        : const Text(
                            'Convert',
                            style: TextStyle(fontSize: 18),
                          )),
              ),
            ],
          ),
          const SizedBox(
            width: 50,
            height: 20,
          ),
          Text(
            conversion,
            style: const TextStyle(fontSize: 26),
            textAlign: TextAlign.center,
          ),
          const Spacer(),
          GestureDetector(
            onTap: () {
              // ignore: deprecated_member_use
              launch('https://github.com/srikanth-rl');
            },
            child: TextButton(
              child: Text(
                '< About Developer >',
                style: TextStyle(
                  fontSize: 20,
                  color: Colors.indigoAccent,
                  decoration: TextDecoration.combine([
                    // TextDecoration.overline,
                    TextDecoration.underline,
                  ]),
                  decorationColor: Colors.black,
                  decorationThickness: 1,
                  decorationStyle: TextDecorationStyle.solid,
                ),
              ),
              onPressed: () {
                // ignore: deprecated_member_use
                launch('https://github.com/srikanth-rl');
              },
            ),
          ),
        ],
      ),
    );
  }
}
