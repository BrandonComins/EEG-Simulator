#ifndef CONTROLTAB_H
#define CONTROLTAB_H

#include <QWidget>

namespace Ui {
class ControlTab;
}

class ControlWidget;
class FlowLayout;

namespace Communication {
class PacketTransceiver;
struct ChannelConstants;
}

class ControlTab : public QWidget
{
    Q_OBJECT

public:
    explicit ControlTab(QWidget *parent = nullptr);
    ~ControlTab();

    /*!
     * \brief add_channel Add a channel control widget;
     */
    void add_channel(const std::string &control_id);

    /*!
     * \brief add_transceiver add a pointer to the packet transceiver;
     */
    void add_transceiver(Communication::PacketTransceiver *packet_transceiver);

    /*!
     * \brief update_channel_constants Update the channel constants in the UI
     * \param constants The channel constants from the device
     */
    void update_channel_constants(const Communication::ChannelConstants &constants);

    /*!
     * \brief on_connection This function is run when a device is connected;
     */
    void on_connection();

    /*!
     * \brief on_disconnect This function is run when a device is disconnected
     */
    void on_disconnect();    

private:
    /*!
     * \brief send_channel_constants Update the channel constants from the device
     */
    void send_channel_constants();

    /*!
     * \brief request_channel_constants Request the channel constants from the device
     */
    void request_channel_constants();

    /*!
     * \brief request_channel_constants Request the channel constants from the device
     */
    void request_channel_constants(int channel_id);

    /*!
     * \brief request_latest_data Request the lastest channel data from the device
     */
    void request_latest_data();

    /*!
     * \brief request_num_channels Request the number of channels from the device
     */
    void request_num_channels();

    QTimer *m_poll_timer; //!< Timer to request new samples
    std::vector<ControlWidget*> m_control_widgets; //!< List of control widgets
    Communication::PacketTransceiver *m_packet_transceiver; //!< Pointer to the packer transceiver
    Ui::ControlTab *ui; //!< Qt Ui
};

#endif // CONTROLTAB_H
